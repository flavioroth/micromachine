//
// Copyright (C) 2020 Joel Jungo, Flavio Roth - All Rights Reserved
//

#ifndef MICROMACHINE_STREAM_SERVER_HPP
#define MICROMACHINE_STREAM_SERVER_HPP

#include <arpa/inet.h>
#include <atomic>
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <functional>
#include <list>
#include <map>
#include <sys/socket.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>
#include <utility>

#include "helpers/check.hpp"

#include "peripherals/iodev.hpp"
#include "io/client_manager.hpp"
#include "io/stream_connection.hpp"

namespace micromachine::system {

/**
 * A Stream server is able to create a unix socket domain (AF_UNIX) server on a given location.
 * The stream server will accept connection and manage a client list.
 * Typically a server could be create to expose iodevices such as usart0 at the following location
 * /tmp/micromachine/<pid>/usart0
 */
class stream_server {
public:
	static constexpr std::string_view DEFAULT_DIRECTORY = "/tmp/micromachine";

private:

	class socket_file {
	private:
		const std::string _path;

	public:
		socket_file(const std::string& path)
			: _path(path) {
				std::filesystem::create_directories(std::filesystem::path(_path).remove_filename());
				remove();
			}

		void remove() {
			std::error_code error;
			std::filesystem::remove(_path, error);
		}

		const std::string& pathname() const {
			return _path;
		}
	};

	std::mutex _close_mutex;

	socket_file _socket_file;
	const int _socket;

	iodev& _iodev;
	iopump _iopump;

	client_manager _clients;

	std::atomic<bool> _cleanup_connections;
	std::atomic<bool> _accept_connections;

	/**
	 * Waitable signal for starting _acceptor_thread
	 */
	waitable_condition _acceptor_thread_ready;
	waitable_condition _cleanup_thread_ready;
	std::thread _cleanup_thread;
	std::thread _acceptor_thread;

	static constexpr int LISTEN_BACKLOG_SIZE = 5;

public:
	stream_server(iodev& device, const std::string& device_name, const std::string& directory)
		: _socket_file(make_socket_path(directory, device_name))
		, _socket(create_and_bind_socket(_socket_file.pathname()))
		, _iodev(device)
		, _iopump(_iodev, std::bind(&stream_server::broadcast, this, std::placeholders::_1))
		, _cleanup_connections(true)
		, _accept_connections(true)
		, _cleanup_thread(std::thread(&stream_server::cleanup_loop, this))
		, _acceptor_thread(std::thread(&stream_server::accept_loop, this)) {

		if(waitable_flag::ok != _acceptor_thread_ready.wait(100ms)) {
			close();
			throw std::runtime_error("acceptor thread didn't start in time");
		}

		if(waitable_flag::ok != _cleanup_thread_ready.wait(100ms)) {
			close();
			throw std::runtime_error("cleanup thread didn't start in time");
		}
	}

	stream_server(iodev& device, const std::string& device_name)
		: stream_server(device, device_name, std::string(DEFAULT_DIRECTORY)) {}

	~stream_server() {
		close();
	}

	const std::string& pathname() const {
		return _socket_file.pathname();
	}

	size_t client_count() {
		return _clients.size();
	}

	void broadcast(uint8_t byte) {
		std::lock_guard<client_manager> lock(_clients);
		for(auto& [_, client] : _clients) {
			client->send(byte);
		}
	}

	void close_all_clients() {
		std::lock_guard<client_manager> lock(_clients);
		for(auto& [_, client] : _clients) {
			client->close();
		}
	}

	void close() {

		std::lock_guard<std::mutex> lock_guard(_close_mutex);

		_iopump.wait_until_flushed();
		_iopump.shutdown();

		_accept_connections = false;

		::shutdown(_socket, SHUT_RDWR);
		::close(_socket);

		if(_acceptor_thread.joinable()) {
			_acceptor_thread.join();
		}

		close_all_clients();
		_clients.wait_no_more_clients();
		_clients.wait_no_more_clients_to_delete();

		_cleanup_connections = false;
		if(_cleanup_thread.joinable()) {
			_cleanup_thread.join();
		}

		_socket_file.remove();
	}

private:

	static std::filesystem::path make_socket_path(const std::string& directory, const std::string& device_name) {
		std::filesystem::path base = directory;
		return base / std::to_string(getpid()) / device_name;
	}

	/**
	 * Helper to create bind and listen to a AF_UNIX socket located in the provided directory.
	 *
	 * create a AF_UNIX socket at /<directory>/<pid>/<device_name>
	 * @param[in] device_name a device name
	 * @param[in] directory the base directory where the AF_UNIX socket will be created
	 * @param[out] pathname the full path (/tmp/micromachine/<pid>/dev0)
	 * @param[out] location AF_UNIX location directory (/tmp/micromachine/<pid>/)
	 * @return the listened socket
	 */
	static int create_and_bind_socket(const std::string& path) {

		int socket = ::socket(AF_UNIX, SOCK_STREAM, 0);
		if(socket == -1) {
			throw std::runtime_error("failed to create client socket: " + std::string(std::strerror(errno)));
		}

		struct sockaddr_un server;

		server.sun_family = AF_UNIX;

		if(path.size() > sizeof(server.sun_path)) {
			std::string message = "unix_socket_domain_path is to big! maximum allowed size is: " +
								  std::to_string(sizeof(server.sun_path));
			throw std::length_error(message);
		}
		strcpy(server.sun_path, path.c_str());

		int r = bind(socket, reinterpret_cast<struct sockaddr*>(&server), sizeof(server));
		if(r) {
			throw std::runtime_error("bind failed. " + std::string(std::strerror(errno)));
		}

		r = listen(socket, LISTEN_BACKLOG_SIZE);
		if(r) {
			throw std::runtime_error("listen failed. " + std::string(std::strerror(errno)));
		}

		return socket;
	}

	void data_receive_from_client_evt(const uint8_t* buffer, size_t size, void*) {
		for(size_t i = 0; i < size; ++i) {
			_iodev.send(buffer[i]);
		}
	}

	void add_new_connection(int client_socket) {
		using namespace std::placeholders;
		auto connection = std::make_unique<stream_connection>(
			client_socket,
			std::bind(&stream_server::client_disconnect_evt, this, _1),
			std::bind(&stream_server::data_receive_from_client_evt, this, _1, _2, _3),
			nullptr);

		// grab a weak reference on the connection
		// so that we can call start after moving the unique ptr
		stream_connection* client = connection.get();
		_clients.add_client(std::move(connection));
		client->start();
	}

	/**
	 * Acceptor thread.
	 *   - Accept incoming connection
	 *   - Create a new stream_connection and push it to the client list
	 *   - flush the disconnected clients list
	 */
	void accept_loop() {

		// ctor synchronization
		_acceptor_thread_ready.set();

		while(_accept_connections) {

			int client_socket = ::accept(_socket, nullptr, nullptr);
			if(client_socket <= 0) {
				continue;
			}

			add_new_connection(client_socket);
		}
	}

	void cleanup_loop() {

		_cleanup_thread_ready.set();

		while(_cleanup_connections) {
			_clients.delete_removed_clients();
			std::this_thread::sleep_for(1ms);
		}
	}

	/**
	 * Called by the client's thread
	 * @param client
	 */
	void client_disconnect_evt(stream_connection& client) {
		_clients.remove_client(client);
	}
};

} // namespace micromachine::system
#endif // MICROMACHINE_STREAM_SERVER_HPP
