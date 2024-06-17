//
// Created by tobi on 07.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_UNIXTRANSPORT_H
#define POLYPHENY_CPP_DRIVER_UNIXTRANSPORT_H

#include "SocketTransport.h"
#include <sys/un.h>

namespace Transport {

    class UnixTransport : public SocketTransport {
    public:
        explicit UnixTransport();
        explicit UnixTransport(std::string socket_file);
        ~UnixTransport() override;
        void connect() override;

    private:
        [[nodiscard]] std::string get_version() const override;
        std::string socket_file;
    };

} // namespace Transport

#endif //POLYPHENY_CPP_DRIVER_UNIXTRANSPORT_H
