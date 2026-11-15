#include "SocketTransport.h"
#include "ConnectionClosedError.h"

#include <cerrno>
#include <limits>

namespace Transport {

    SocketTransport::~SocketTransport() {
        close_socket();
    }

    void SocketTransport::send_message(const std::string &message) {
        std::lock_guard<std::mutex> lock(write_mutex);
        uint64_t message_length = message.size();
        char header[sizeof(message_length)]{};
        for (auto &byte : header) {
            byte = static_cast<char>(message_length & 0xff);
            message_length >>= 8;
        }
        send_all(header, sizeof(header));
        send_all(message.data(), message.size());
    }

    std::string SocketTransport::receive_message() {
        unsigned char header[sizeof(uint64_t)]{};
        receive_all(reinterpret_cast<char *>(header), sizeof(header));
        uint64_t message_length = 0;
        for (size_t i = sizeof(header); i > 0; --i) {
            message_length = (message_length << 8) | header[i - 1];
        }
        if (message_length > std::numeric_limits<int>::max()) {
            throw std::runtime_error("Prism frame exceeds the maximum Protobuf message size");
        }
        std::string message(message_length, '\0');
        receive_all(message.data(), message.size());
        return message;
    }

    void SocketTransport::send_all(const char *data, size_t size) {
#ifdef MSG_NOSIGNAL
        constexpr int flags = MSG_NOSIGNAL;
#else
        constexpr int flags = 0;
#endif
        while (size > 0) {
            const auto sent = send(socket_fd, data, size, flags);
            if (sent < 0 && errno == EINTR) {
                continue;
            }
            if (sent <= 0) {
                throw std::runtime_error("Failed to send message: " + std::string(strerror(errno)));
            }
            data += sent;
            size -= sent;
        }
    }

    void SocketTransport::receive_all(char *data, size_t size) {
        while (size > 0) {
            const auto received = recv(socket_fd, data, size, 0);
            if (received < 0 && errno == EINTR) {
                continue;
            }
            if (received == 0) {
                throw Errors::ConnectionClosedError("Connection closed while receiving a Prism frame");
            }
            if (received < 0) {
                throw std::runtime_error("Failed to receive message: " + std::string(strerror(errno)));
            }
            data += received;
            size -= received;
        }
    }

    void SocketTransport::exchange_version() {
        int8_t incoming_version_length = 0;
        receive_all(reinterpret_cast<char *>(&incoming_version_length), sizeof(incoming_version_length));

        if (incoming_version_length <= 0) {
            throw std::runtime_error("Invalid version length: " + std::string(strerror(errno)));
        }

        std::string incoming_version(incoming_version_length, '\0');
        receive_all(incoming_version.data(), incoming_version.size());

        if (incoming_version != get_version()) {
            throw std::runtime_error("Unsupported version: '" + incoming_version + "' expected '" + get_version() + "'");
        }

        auto version_length = static_cast<int8_t>(get_version().size());
        std::string version_message = std::string(1, version_length) + get_version();
        send_all(version_message.data(), version_message.size());
    }

    void SocketTransport::shutdown() noexcept {
        if (socket_fd >= 0) {
#ifdef _WIN32
            ::shutdown(socket_fd, SD_BOTH);
#else
            ::shutdown(socket_fd, SHUT_RDWR);
#endif
        }
    }

    void SocketTransport::close_socket() {
        if (socket_fd < 0) {
            return;
        }
#ifdef _WIN32
        closesocket(socket_fd);
#else
        close(socket_fd);
#endif
        socket_fd = -1;
    }

} // namespace Transport
