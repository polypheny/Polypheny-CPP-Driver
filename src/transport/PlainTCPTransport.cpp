#include "PlainTCPTransport.h"

#include <utility>

namespace Transport {

    const std::string VERSION = "plain-v1@polypheny.com\n";

    PlainTCPTransport::PlainTCPTransport() : host("localhost"), port(20590) {}

    PlainTCPTransport::PlainTCPTransport(std::string host, uint16_t port) : host(std::move(host)), port(port) {}


    PlainTCPTransport::~PlainTCPTransport() {
        close_socket();
    }

    std::string PlainTCPTransport::get_version() const {
        return VERSION;
    }

    void PlainTCPTransport::connect() {
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
        if (::connect(socket_fd, res->ai_addr, res->ai_addrlen) == -1) {
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
} // namespace Transport
