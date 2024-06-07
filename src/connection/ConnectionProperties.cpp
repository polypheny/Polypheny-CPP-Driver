#include "src/connection/ConnectionProperties.h"

#include <utility>

namespace Connection {

    ConnectionProperties::ConnectionProperties(std::string host,
                                               uint16_t port,
                                               std::string username,
                                               std::string password,
                                               std::string default_nspace,
                                               bool is_password_required,
                                               bool is_auto_commit)
            : host(std::move(host)), port(port), username(std::move(username)), password(std::move(password)),
              default_namespace(std::move(default_nspace)), is_password_required(is_password_required),
              is_auto_commit(is_auto_commit) {}

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

    void ConnectionProperties::set_is_password_required(bool is_password_required) {
        this->is_password_required = is_password_required;
    }

    bool ConnectionProperties::get_is_password_required() const {
        return is_password_required;
    }

}
