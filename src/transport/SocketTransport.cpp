#include "SocketTransport.h"

namespace Transport {

    SocketTransport::~SocketTransport() {
        close_socket();
    }

    void SocketTransport::send_message(const std::string &message) {
        std::lock_guard<std::mutex> lock(write_mutex);
        auto message_length = static_cast<uint64_t>(message.size());

        std::vector<uint8_t> buffer(sizeof(message_length) + message.size());
        std::memcpy(buffer.data(), &message_length, sizeof(message_length));
        std::memcpy(buffer.data() + sizeof(message_length), message.data(), message.size());

        if (send(socket_fd, reinterpret_cast<const char *>(buffer.data()), buffer.size(), 0) == -1) {
            throw std::runtime_error("Failed to send message: " + std::string(strerror(errno)));
        }
    }

    std::string SocketTransport::receive_message() {
        uint64_t message_length = 0;
        ssize_t bytes_received = recv(socket_fd, reinterpret_cast<char *>(&message_length), sizeof(message_length), 0);
        if (bytes_received == 0) {
            throw std::runtime_error("Connection closed while receiving message length: " + std::string(strerror(errno)));
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

    void SocketTransport::exchange_version() {
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

        if (incoming_version != get_version()) {
            throw std::runtime_error("Unsupported version: '" + incoming_version + "' expected '" + get_version() + "'");
        }

        auto version_length = static_cast<int8_t>(get_version().size());
        std::string version_message = std::string(1, version_length) + get_version();
        if (send(socket_fd, version_message.c_str(), version_message.size(), 0) == -1) {
            throw std::runtime_error("Failed to send version: " + std::string(strerror(errno)));
        }
    }

    void SocketTransport::close_socket() const {
#ifdef _WIN32
        closesocket(socket_fd);
#else
        close(socket_fd);
#endif
    }

} // namespace Transport
