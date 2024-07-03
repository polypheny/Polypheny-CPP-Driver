#ifndef POLYPHENY_CPP_DRIVER_CONNECTIONPROPERTIES_H
#define POLYPHENY_CPP_DRIVER_CONNECTIONPROPERTIES_H

#include <string>
#include <cstdint>

namespace Connection {

    /**
     * This class is a container for various properties related to a connection with a Polypheny instance independent of the type of transport used.
     * Transports refer to the way the driver communicates with the Polypheny instance. Currently plain TCP and plain UNIX sockets are supported. Their options can be configured in the corresponding classes.
     * Connection properties on the other hand only deal with user preferences and authentication. Parameters like username, password or options like autocommit are handled.
     * Connection properties can be reused. Modifications made to the properties don't affect connections already established using this properties object..
     */
    class ConnectionProperties {
    private:
        std::string username;
        std::string password;
        std::string default_namespace;
        bool is_password_required;
        bool is_auto_commit;

    public:
        /**
         * @brief Constructs a ConnectionProperties object with optional parameters.
         * Default values are used for the omitted parameters. Setters can be used to modify parameters later on.
         *
         * @param username The username for authentication (default is "pa").
         * @param password The password for authentication (default is an empty string).
         * @param default_nspace The default namespace for the connection (default is "public").
         * @param is_password_required Indicates if a password is required (default is true).
         * @param is_auto_commit Indicates if auto-commit is enabled (default is true).
         */
        explicit ConnectionProperties(std::string username = "pa",
                                      std::string password = "",
                                      std::string default_nspace = "public",
                                      bool is_password_required = true,
                                      bool is_auto_commit = true);

        /**
         * @brief Gets the username to be used on connection.
         *
         * @return The username.
         */
        [[nodiscard]] std::string get_username() const;

        /**
         * @brief Sets the username to be used for authentication.
         *
         * @param username The new username.
         */
        void set_username(const std::string &username);

        /**
         * @brief Sets whether a password is used for authentication.
         *
         * If this is set to false on a connection requiring a password an error will be thrown when the connection is established.
         *
         * @param is_password_required True if a password is required, false otherwise.
         */
        void set_is_password_required(bool is_password_required);

        /**
           * @brief Gets whether a password should be used for authentication.
           *
           * @return True if a password is required, false otherwise.
           */
        [[nodiscard]] bool get_is_password_required() const;

        /**
         * @brief Gets the password that will be used for the connection.
         *
         * @return The password.
         */
        [[nodiscard]] std::string get_password() const;

        /**
         * @brief Sets the password to be used for authentication.
         *
         * If "is_password_required" is set to false, this value will be ignored.
         *
         * @param password The new password.
         */
        void set_password(const std::string &password);

        /**
         * @brief Gets the default namespace to be used for the connection.
         *
         * @return The default namespace.
         */
        [[nodiscard]] std::string get_default_namespace() const;

        /**
         * @brief Sets the default namespace to be used for the connection.
         *
         * @param default_nspace The new default namespace.
         */
        void set_default_namespace(const std::string &default_nspace);

        /**
         * @brief Gets whether auto-commit should be enabled.
         *
         * @return True if auto-commit is enabled, false otherwise.
         */
        [[nodiscard]] bool get_is_auto_commit() const;

        /**
         * @brief Sets whether auto-commit should be enabled enabled.
         *
         * @param is_auto_commit True to enable auto-commit, false to disable it.
         */
        void set_is_auto_commit(bool is_auto_commit);
    };

} // Connection

#endif //POLYPHENY_CPP_DRIVER_CONNECTIONPROPERTIES_H
