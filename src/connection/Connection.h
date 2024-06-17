#ifndef POLYPHENY_CPP_DRIVER_CONNECTION_H
#define POLYPHENY_CPP_DRIVER_CONNECTION_H

#include <list>
#include "ConnectionProperties.h"
#include "PrismInterfaceClient.h"

namespace Results {
    class RelationalResult;

    class DocumentResult;

    class GraphResult;
}

namespace Connection {
    // forward declaration, include in .cpp
    class Cursor;

    class Connection {
    private:
        Communication::PrismInterfaceClient prism_interface_client;
        ConnectionProperties connection_properties;
        std::list<Cursor> cursors;

        static ConnectionProperties
        build_connection_properties(const std::string &username, const std::string &password);

    protected:
        Communication::PrismInterfaceClient &get_prism_interface_client();

        [[nodiscard]] const ConnectionProperties &get_connection_properties() const;

        friend class Cursor;

        friend class Results::RelationalResult;

        friend class Results::DocumentResult;

        friend class Results::GraphResult;


    public:
        explicit Connection(ConnectionProperties &connection_properties);

        ~ Connection();

        Connection(const std::string &host, const std::string &user, const std::string &password);

        Connection(ConnectionProperties& connection_properties, std::unique_ptr<Transport::Transport> &&transport);
    };

} // Connection

#endif //POLYPHENY_CPP_DRIVER_CONNECTION_H
