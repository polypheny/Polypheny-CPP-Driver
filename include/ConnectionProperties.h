#ifndef POLYPHENY_CPP_DRIVER_CONNECTIONPROPERTIES_H
#define POLYPHENY_CPP_DRIVER_CONNECTIONPROPERTIES_H

#include <string>
#include <cstdint>

namespace Connection {

    class ConnectionProperties {
    private:
        std::string host;
        uint16_t port;
        std::string username;
        std::string password;
        std::string default_namespace;
        bool is_password_required;
        bool is_auto_commit;

    public:
        explicit ConnectionProperties(std::string host = "localhost",
                                      uint16_t port = 20590,
                                      std::string username = "pa",
                                      std::string password = "",
                                      std::string default_nspace = "public",
                                      bool is_password_required = true,
                                      bool is_auto_commit = true);


        [[nodiscard]] std::string get_host() const;

        void set_host(const std::string &host);

        [[nodiscard]] uint16_t get_port() const;

        void set_port(uint16_t port);

        [[nodiscard]] std::string get_username() const;

        void set_username(const std::string &username);

        void set_is_password_required(bool is_password_required);

        [[nodiscard]] bool get_is_password_required() const;

        [[nodiscard]] std::string get_password() const;

        void set_password(const std::string &password);

        [[nodiscard]] std::string get_default_namespace() const;

        void set_default_namespace(const std::string &default_nspace);

        [[nodiscard]] bool get_is_auto_commit() const;

        void set_is_auto_commit(bool is_auto_commit);
    };

} // Connection

#endif //POLYPHENY_CPP_DRIVER_CONNECTIONPROPERTIES_H
