//
// Created by tobi on 17.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_SECURETCPTRANSPORT_H
#define POLYPHENY_CPP_DRIVER_SECURETCPTRANSPORT_H

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>

#include "SocketTransport.h"

namespace Transport {

    class SecureTcpTransport : public SocketTransport {
    public:
        explicit SecureTcpTransport(std::string  cert_file = "", std::string  key_file = "", std::string host = "localhost", uint16_t port = 20590);

        ~SecureTcpTransport() override;
        void connect() override;
        void send_message(const std::string &message) override;
        std::string receive_message() override;

    private:
        [[nodiscard]] std::string get_version() const override;
        std::string host;
        uint16_t port;
        std::string cert_file;
        std::string key_file;
        SSL_CTX* ctx;
        SSL* ssl;

        static void init_openssl();
        static void cleanup_openssl();
        static SSL_CTX* create_context();
        void configure_context(SSL_CTX* ctx);
        void disconnect();
    };


} // Transport

#endif //POLYPHENY_CPP_DRIVER_SECURETCPTRANSPORT_H
