#include "PlainTransport.h"

namespace Communication {
    const std::string PlainTransport::VERSION = "plain-v1@polypheny.com\n";

    PlainTransport::PlainTransport(const std::string &host, uint16_t port) {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
#endif
        addrinfo hints{}, *res;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res) != 0) {
            throw std::runtime_error("getaddrinfo failed: " + std::string(strerror(errno)));
        }
        socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (socket_fd == -1) {
            freeaddrinfo(res);
            throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
        }
        if (connect(socket_fd, res->ai_addr, res->ai_addrlen) == -1) {
            freeaddrinfo(res);
            close_socket();
            throw std::runtime_error("Failed to connect to server: " + std::string(strerror(errno)));
        }
        freeaddrinfo(res);

        int flag = 1;
        if (setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)) < 0) {
            close_socket();
            throw std::runtime_error("Failed to set TCP_NODELAY: " + std::string(strerror(errno)));
        }

        exchange_version();
    }

    PlainTransport::~PlainTransport() {
        close_socket();
#ifdef _WIN32
        WSACleanup();
#endif
    }

    void PlainTransport::close_socket() {
#ifdef _WIN32
        closesocket(socket_fd);
#else
        close(socket_fd);
#endif
    }

    void PlainTransport::send_message(const std::string &message) {
        std::lock_guard<std::mutex> lock(write_mutex);
        auto message_length = static_cast<uint64_t>(message.size());

        std::vector<uint8_t> buffer(sizeof(message_length) + message.size());
        std::memcpy(buffer.data(), &message_length, sizeof(message_length));
        std::memcpy(buffer.data() + sizeof(message_length), message.data(), message.size());

        if (send(socket_fd, reinterpret_cast<const char *>(buffer.data()), buffer.size(), 0) == -1) {
            throw std::runtime_error("Failed to send message: " + std::string(strerror(errno)));
        }
    }

    std::string PlainTransport::receive_message() {
        uint64_t message_length = 0;
        ssize_t bytes_received = recv(socket_fd, reinterpret_cast<char *>(&message_length),
                                      sizeof(message_length), 0);
        if (bytes_received == 0) {
            throw Errors::ConnectionClosedError("Connection closed while receiving message length: " + std::string(strerror(errno)));
        } else if (bytes_received < 0) {
            throw std::runtime_error("Failed to receive message length: " + std::string(strerror(errno)));
        }

        std::string message(message_length, '\0');
        bytes_received = recv(socket_fd, &message[0], message_length, 0);

        if (bytes_received == 0) {
            throw Errors::ConnectionClosedError("Connection closed while receiving message: " + std::string(strerror(errno)));
        } else if (bytes_received < 0) {
            throw std::runtime_error("Failed to receive message: " + std::string(strerror(errno)));
        }

        return message;
    }

    void PlainTransport::exchange_version() {
        int8_t incoming_version_length = 0;
        if (recv(socket_fd, reinterpret_cast<char *>(&incoming_version_length), sizeof(incoming_version_length), 0) <=
            0) {
            throw std::runtime_error("Failed to receive version length: " + std::string(strerror(errno)));
        }

        if (incoming_version_length <= 0) {
            throw std::runtime_error("Invalid version length: " + std::string(strerror(errno)));
        }

        std::string incoming_version(incoming_version_length, '\0');
        if (recv(socket_fd, &incoming_version[0], incoming_version_length, 0) <= 0) {
            throw std::runtime_error("Failed to receive version: " + std::string(strerror(errno)));
        }

        if (incoming_version != VERSION) {
            throw std::runtime_error("Unsupported version: '" + incoming_version + "' expected '" + VERSION + "'");
        }

        auto version_length = static_cast<int8_t>(VERSION.size());
        std::string version_message = std::string(1, version_length) + VERSION;
        if (send(socket_fd, version_message.c_str(), version_message.size(), 0) == -1) {
            throw std::runtime_error("Failed to send version: " + std::string(strerror(errno)));
        }
    }
}
