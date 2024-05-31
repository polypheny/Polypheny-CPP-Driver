#ifndef POLYPHENY_CPP_DRIVER_CONNECTION_H
#define POLYPHENY_CPP_DRIVER_CONNECTION_H

#include "ConnectionProperties.h"
#include "../src/PrismInterfaceClient.h"
#include "src/Cursor.h"

namespace Connection {

    class Connection {
    private:
        Communication::PrismInterfaceClient prism_interface_client;
        ConnectionProperties connection_properties;
        std::list<Cursor> cursors;
    public:
        Connection(ConnectionProperties &connection_properties);

        Connection(const std::string &host, const std::string &user, const std::string &password);

        const Communication::PrismInterfaceClient &get_prism_interface_client() const;

        const ConnectionProperties &get_connection_properties() const;

        static ConnectionProperties
        build_connection_properties(const std::string &host, const std::string &username, const std::string &password);
    };

} // Connection

#endif //POLYPHENY_CPP_DRIVER_CONNECTION_H
