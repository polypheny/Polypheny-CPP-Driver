//
// Created by tobi on 17.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_PLAINTCPTRANSPORT_H
#define POLYPHENY_CPP_DRIVER_PLAINTCPTRANSPORT_H

#include "SocketTransport.h"

namespace Transport {

    class PlainTcpTransport : public SocketTransport {
    public:
        PlainTcpTransport();
        explicit PlainTcpTransport(std::string host, uint16_t port = 20590);
        ~PlainTcpTransport() override;
        void connect() override;

    private:
        [[nodiscard]] std::string get_version() const override;
        std::string host;
        uint16_t port;
    };

} // namespace Transport

#endif //POLYPHENY_CPP_DRIVER_PLAINTCPTRANSPORT_H
