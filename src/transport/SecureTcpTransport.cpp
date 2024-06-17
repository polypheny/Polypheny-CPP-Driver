#include "SecureTcpTransport.h"
#include <utility>
#include <stdexcept>
#include <cstring>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace Transport {

    const std::string VERSION = "ssl-v1@polypheny.com\n";

    SecureTcpTransport::SecureTcpTransport(std::string  cert_file, std::string  key_file, std::string host, uint16_t port)
            : host(std::move(host)), port(port), cert_file(std::move(cert_file)), key_file(std::move(key_file)), ctx(nullptr), ssl(nullptr) {
        init_openssl();
        ctx = create_context();
        configure_context(ctx);
    }


    SecureTcpTransport::~SecureTcpTransport() {
        disconnect();
        cleanup_openssl();
    }

    void SecureTcpTransport::init_openssl() {
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();
    }

    void SecureTcpTransport::cleanup_openssl() {
        EVP_cleanup();
    }

    SSL_CTX* SecureTcpTransport::create_context() {
        const SSL_METHOD* method;
        SSL_CTX* ctx;

        method = SSLv23_client_method();

        ctx = SSL_CTX_new(method);
        if (!ctx) {
            perror("Unable to create SSL context");
            ERR_print_errors_fp(stderr);
            exit(EXIT_FAILURE);
        }

        return ctx;
    }

    void SecureTcpTransport::configure_context(SSL_CTX* ctx) {
        if (!cert_file.empty() && !key_file.empty()) {
            if (SSL_CTX_use_certificate_file(ctx, cert_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
                ERR_print_errors_fp(stderr);
                throw std::runtime_error("Failed to load certificate");
            }
            if (SSL_CTX_use_PrivateKey_file(ctx, key_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
                ERR_print_errors_fp(stderr);
                throw std::runtime_error("Failed to load private key");
            }
        }
    }

    void SecureTcpTransport::connect() {
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

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, socket_fd);

        if (SSL_connect(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("Failed to establish SSL connection");
        } else {
            printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
        }

        int flag = 1;
        if (setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)) < 0) {
            close_socket();
            throw std::runtime_error("Failed to set TCP_NODELAY: " + std::string(strerror(errno)));
        }

        exchange_version();
    }

    void SecureTcpTransport::send_message(const std::string &message) {
        std::lock_guard<std::mutex> lock(write_mutex);
        auto message_length = static_cast<uint64_t>(message.size());

        std::vector<uint8_t> buffer(sizeof(message_length) + message.size());
        std::memcpy(buffer.data(), &message_length, sizeof(message_length));
        std::memcpy(buffer.data() + sizeof(message_length), message.data(), message.size());

        if (SSL_write(ssl, buffer.data(), buffer.size()) <= 0) {
            throw std::runtime_error("Failed to send message: " + std::string(ERR_error_string(ERR_get_error(), nullptr)));
        }
    }

    std::string SecureTcpTransport::receive_message() {
        uint64_t message_length = 0;
        ssize_t bytes_received = SSL_read(ssl, reinterpret_cast<char *>(&message_length), sizeof(message_length));
        if (bytes_received == 0) {
            throw std::runtime_error("Connection closed while receiving message length: " + std::string(ERR_error_string(ERR_get_error(), nullptr)));
        } else if (bytes_received < 0) {
            throw std::runtime_error("Failed to receive message length: " + std::string(ERR_error_string(ERR_get_error(), nullptr)));
        }

        std::string message(message_length, '\0');
        bytes_received = SSL_read(ssl, &message[0], message_length);

        if (bytes_received == 0) {
            throw std::runtime_error("Connection closed while receiving message: " + std::string(ERR_error_string(ERR_get_error(), nullptr)));
        } else if (bytes_received < 0) {
            throw std::runtime_error("Failed to receive message: " + std::string(ERR_error_string(ERR_get_error(), nullptr)));
        }

        return message;
    }

    void SecureTcpTransport::disconnect() {
        if (ssl) {
            SSL_shutdown(ssl);
            SSL_free(ssl);
        }
        if (socket_fd >= 0) {
            close_socket();
        }
        if (ctx) {
            SSL_CTX_free(ctx);
        }
    }

    std::string SecureTcpTransport::get_version() const {
        return VERSION;
    }

} // namespace Transport
