//
// Created by tobi on 07.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_UNIXTRANSPORT_H
#define POLYPHENY_CPP_DRIVER_UNIXTRANSPORT_H

#include <string>
#include <vector>
#include <mutex>
#include <stdexcept>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "PlainTransport.h"

namespace Transport {

    class UnixTransport {
    public:
        static const std::string VERSION;

        UnixTransport(const std::string &path);
        ~UnixTransport();

        void send_message(const std::string &message);
        std::string receive_message();

    private:
        int socket_fd;
        std::mutex write_mutex;

        void close_socket();
        void exchange_version();
    };

} // Communication

#endif //POLYPHENY_CPP_DRIVER_UNIXTRANSPORT_H
