#include "ConnectionProperties.h"

namespace Connection {

    ConnectionProperties::ConnectionProperties(const std::string &host,
                                               uint16_t port,
                                               const std::string &username,
                                               const std::string &password,
                                               const std::string &default_nspace,
                                               bool is_auto_commit)
            : host(host), port(port), username(username), password(password),
              default_namespace(default_nspace), is_auto_commit(is_auto_commit) {}

    std::string ConnectionProperties::get_host() const {
        return host;
    }

    void ConnectionProperties::set_host(const std::string &host) {
        this->host = host;
    }

    uint16_t ConnectionProperties::get_port() const {
        return port;
    }

    void ConnectionProperties::set_port(uint16_t port) {
        this->port = port;
    }

    std::string ConnectionProperties::get_username() const {
        return username;
    }

    void ConnectionProperties::set_username(const std::string &username) {
        this->username = username;
    }

    std::string ConnectionProperties::get_password() const {
        return password;
    }

    void ConnectionProperties::set_password(const std::string &password) {
        this->password = password;
    }

    std::string ConnectionProperties::get_default_namespace() const {
        return default_namespace;
    }

    void ConnectionProperties::set_default_namespace(const std::string &default_nspace) {
        this->default_namespace = default_nspace;
    }

    bool ConnectionProperties::get_is_auto_commit() const {
        return is_auto_commit;
    }

    void ConnectionProperties::set_is_auto_commit(bool is_auto_commit) {
        this->is_auto_commit = is_auto_commit;
    }

}
