#ifndef POLYPHENY_CPP_DRIVER_CONNECTION_H
#define POLYPHENY_CPP_DRIVER_CONNECTION_H

#include <list>
#include "ConnectionProperties.h"
#include "PrismInterfaceClient.h"

namespace Connection {
    // forward declaration, include in .cpp
    class Cursor;

    class Connection {
    private:
        Communication::PrismInterfaceClient prism_interface_client;
        ConnectionProperties connection_properties;
        std::list<Cursor> cursors;

        static ConnectionProperties
        build_connection_properties(const std::string &host, const std::string &username, const std::string &password);

    public:
        Connection(ConnectionProperties &connection_properties);

        Connection(const std::string &host, const std::string &user, const std::string &password);

        Communication::PrismInterfaceClient &get_prism_interface_client();

        const ConnectionProperties &get_connection_properties() const;


    };

} // Connection

#endif //POLYPHENY_CPP_DRIVER_CONNECTION_H
