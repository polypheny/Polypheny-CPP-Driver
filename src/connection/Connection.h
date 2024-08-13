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

    /**
     * @brief Represents a connection to the Polypheny database.
     *
     * This class represents a connection a Polypheny instance. It further provides all methods required to establish such a connection.
     * Methods are provided to establish a connection based on defaults or a custom configuration.
     */
    class Connection {
    private:
        std::shared_ptr<Communication::PrismInterfaceClient> prism_interface_client;
        ConnectionProperties connection_properties;
        std::list<Cursor> cursors;

        static ConnectionProperties
        build_connection_properties(const std::string &username, const std::string &password);

    protected:
        std::shared_ptr<Communication::PrismInterfaceClient> get_prism_interface_client();

        [[nodiscard]] const ConnectionProperties &get_connection_properties() const;

        friend class Cursor;

        friend class Results::RelationalResult;

        friend class Results::DocumentResult;

        friend class Results::GraphResult;


    public:
        /**
         * @brief Constructs a Connection object with the given connection properties.
         *
         * @param connection_properties The properties used to configure the connection.
         */
        explicit Connection(ConnectionProperties &connection_properties);

        /**
         * @brief Destroys the Connection object thus closing the connection.
         */
        ~ Connection();

        /**
         * @brief Constructs a Connection object using the specified host, user, and password.
         *
         * @param host The hostname or IP address of the Polypheny instance.
         * @param user The username of the Polypheny user for which this connection should be opened.
         * @param password The password of the above Polypheny user for authentication.
         */
        Connection(const std::string &host, const std::string &user, const std::string &password);

        /**
         * @brief Constructs a Connection object with the given connection properties and transport.
         *
         * @param connection_properties The properties used to configure the connection.
         * @param transport A unique pointer to a transport object used for communication with the Polypheny instance.
         */
        Connection(ConnectionProperties& connection_properties, std::unique_ptr<Transport::Transport> &&transport);
    };

} // Connection

#endif //POLYPHENY_CPP_DRIVER_CONNECTION_H
