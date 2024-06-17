//
// Created by tobi on 17.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_PLAINTCPTRANSPORT_H
#define POLYPHENY_CPP_DRIVER_PLAINTCPTRANSPORT_H

#include "SocketTransport.h"

namespace Transport {

    class PlainTCPTransport : public SocketTransport {
    public:
        PlainTCPTransport(const std::string &host, uint16_t port);
        ~PlainTCPTransport() override;

    private:
        [[nodiscard]] std::string get_version() const override;
    };

} // namespace Transport

#endif //POLYPHENY_CPP_DRIVER_PLAINTCPTRANSPORT_H
