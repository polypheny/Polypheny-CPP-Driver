#include "UnixTransport.h"

namespace Communication {

    const std::string UnixTransport::VERSION = "unix-v1@polypheny.com\n";

    UnixTransport::UnixTransport(const std::string &path) {
#ifdef _WIN32
        throw std::runtime_error("UnixTransport is not supported on windows.");
#else
        socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (socket_fd == -1) {
            throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
        }

        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);

        if (connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
            close_socket();
            throw std::runtime_error("Failed to connect to server: " + std::string(strerror(errno)));
        }

        exchange_version();
#endif
    }

    UnixTransport::~UnixTransport() {
        close_socket();
    }

    void UnixTransport::close_socket() {
        close(socket_fd);
    }

    void UnixTransport::send_message(const std::string &message) {
        std::lock_guard<std::mutex> lock(write_mutex);
        auto message_length = static_cast<uint64_t>(message.size());

        std::vector<uint8_t> buffer(sizeof(message_length) + message.size());
        std::memcpy(buffer.data(), &message_length, sizeof(message_length));
        std::memcpy(buffer.data() + sizeof(message_length), message.data(), message.size());

        if (send(socket_fd, reinterpret_cast<const char *>(buffer.data()), buffer.size(), 0) == -1) {
            throw std::runtime_error("Failed to send message: " + std::string(strerror(errno)));
        }
    }

    std::string UnixTransport::receive_message() {
        uint64_t message_length = 0;
        ssize_t bytes_received = recv(socket_fd, reinterpret_cast<char *>(&message_length), sizeof(message_length), 0);
        if (bytes_received == 0) {
            throw std::runtime_error("Connection closed while receiving message length" + std::string(strerror(errno)));
        } else if (bytes_received < 0) {
            throw std::runtime_error("Failed to receive message length: " + std::string(strerror(errno)));
        }

        std::string message(message_length, '\0');
        bytes_received = recv(socket_fd, &message[0], message_length, 0);

        if (bytes_received == 0) {
            throw std::runtime_error("Connection closed while receiving message: " + std::string(strerror(errno)));
        } else if (bytes_received < 0) {
            throw std::runtime_error("Failed to receive message: " + std::string(strerror(errno)));
        }

        return message;
    }

    void UnixTransport::exchange_version() {
        int8_t incoming_version_length = 0;
        if (recv(socket_fd, reinterpret_cast<char *>(&incoming_version_length), sizeof(incoming_version_length), 0) <= 0) {
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