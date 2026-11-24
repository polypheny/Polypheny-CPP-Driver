#include "Connection.h"
#include "Cursor.h"

namespace Connection {

    Connection::Connection(ConnectionProperties &connection_properties)
            : Connection(connection_properties, std::make_unique<Transport::PlainTcpTransport>()) {
    }

    Connection::Connection(ConnectionProperties &connection_properties, std::unique_ptr<Transport::Transport> &&transport)
            : connection_properties(connection_properties),
              prism_interface_client(std::make_shared<Communication::PrismInterfaceClient>(this->connection_properties, std::move(transport))) {
    }

    Connection::Connection(const std::string &host, const std::string &user, const std::string &password)
            : connection_properties(build_connection_properties(user, password)),
              prism_interface_client(std::make_shared<Communication::PrismInterfaceClient>(connection_properties, std::make_unique<Transport::PlainTcpTransport>(host))) {
    }


    Connection::~Connection() {
        try {
            prism_interface_client->disconnect_and_close();
        } catch (const std::exception &) {
            // The transport is already closed if the final request failed.
        }
    }


    ConnectionProperties Connection::build_connection_properties(const std::string &username,
                                                                 const std::string &password) {
        ConnectionProperties connection_properties;
        connection_properties.set_username(username);
        connection_properties.set_password(password);
        return connection_properties;
    }

    std::shared_ptr<Communication::PrismInterfaceClient> Connection::get_prism_interface_client() {
        return prism_interface_client;
    }

    const ConnectionProperties &Connection::get_connection_properties() const {
        return connection_properties;
    }

    std::pair<int, int> Connection::get_server_api_version() const noexcept {
        return prism_interface_client->get_server_api_version();
    }

    void Connection::commit() {
        prism_interface_client->commit_transaction();
    }

    void Connection::rollback() {
        prism_interface_client->rollback_transaction();
    }

    void Connection::set_client_info_properties(const std::map<std::string, std::string> &properties) {
        prism_interface_client->set_client_info_properties(properties);
    }

    org::polypheny::prism::EntitiesResponse
    Connection::search_entities(const std::string &namespace_name, const std::optional<std::string> &pattern) {
        return prism_interface_client->search_entities(namespace_name, pattern);
    }

    std::vector<Results::DocumentMetadata>
    Connection::get_document_collections(const std::string &namespace_name, const std::optional<std::string> &pattern) {
#if POLYPHENY_PRISM_STREAMING
        throw std::runtime_error("Document collection metadata is unavailable in the Prism 5.1 streaming schema");
#else
        const auto [major, minor] = get_server_api_version();
        if (major != 2 || minor < 2) {
            throw std::runtime_error("Document collection metadata requires Prism 2.2 server support");
        }
        std::vector<Results::DocumentMetadata> collections;
        const auto response = search_entities(namespace_name, pattern);
        for (const auto &entity : response.entities()) {
            if (!entity.has_document()) {
                continue;
            }
            const auto &document = entity.document();
            Results::DocumentMetadata collection;
            collection.namespace_name = document.namespace_name();
            collection.collection_name = document.collection_name();
            for (const auto &field : document.fields()) {
                collection.fields.push_back({field.field_path(), field.type_name(), field.is_optional()});
            }
            if (document.has_sample_document_json()) {
                collection.sample_document_json = document.sample_document_json();
            }
            for (const auto &index : document.document_indexes()) {
                collection.document_indexes.push_back({index.field_path(), index.index_type(), index.unique()});
            }
            collections.push_back(std::move(collection));
        }
        return collections;
#endif
    }
}
