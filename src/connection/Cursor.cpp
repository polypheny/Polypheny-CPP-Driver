#include "Cursor.h"
#include "src/connection/Connection.h"
#include <iostream>

namespace Connection {

    // INFO: the statement_id set here is a placeholder value ignored as is_statement_id_set == false
    Cursor::Cursor(Connection &connection) : connection(connection), statement_id(0), is_statement_id_set(false) {}

    Cursor::Cursor(const Cursor &other)
            : connection(other.connection),
              statement_id(other.statement_id),
              is_statement_id_set(other.is_statement_id_set) {}

    Cursor &Cursor::operator=(const Cursor &other) {
        if (this == &other) {
            return *this;
        }
        statement_id = other.statement_id;
        is_statement_id_set = other.is_statement_id_set;
        return *this;
    }

    Cursor::~Cursor() {
        // TODO: implement d'tor if needed or remove
    }

    void Cursor::reset_statement() {
        if (is_statement_id_set) {
            is_statement_id_set = false;
        }
    }

    std::unique_ptr<Results::Result> Cursor::get_result_from_frame(const org::polypheny::prism::Frame &frame) {
        bool is_last = frame.is_last();
        switch (frame.result_case()) {
            case org::polypheny::prism::Frame::ResultCase::kRelationalFrame:
                return std::make_unique<Results::RelationalResult>(frame.relational_frame(), is_last, this);
            case org::polypheny::prism::Frame::ResultCase::kDocumentFrame:
                return std::make_unique<Results::DocumentResult>(frame.document_frame(), is_last, this);
            case org::polypheny::prism::Frame::ResultCase::kGraphFrame:
                return std::make_unique<Results::GraphResult>(frame.graph_frame(), is_last, this);
            default:
                throw std::runtime_error("Frame of illegal result type encountered");
        }
    }

    std::unique_ptr<Results::Result>
    Cursor::execute(const std::string &language, const std::string &statement, const std::string &nspace) {
        reset_statement();
        auto callback = std::make_shared<Communication::CallbackQueue>();
        connection.get_prism_interface_client().execute_unparameterized_statement(nspace, language, statement, callback);

        while (true) {
            org::polypheny::prism::Response response = callback->take_next();
            if (!response.has_statement_response()) {
                throw std::runtime_error("Received illegal response type on statement execution");
            }
            const org::polypheny::prism::StatementResponse& statement_response = response.statement_response();

            if (!is_statement_id_set) {
                statement_id = statement_response.statement_id();
                is_statement_id_set = true;
            }

            if (!statement_response.has_result()) {
                continue;
            }

            callback->await_completion();

            if (!statement_response.result().has_frame()) {
                return std::make_unique<Results::ScalarResult>(statement_response.result().scalar());
            }

            return get_result_from_frame(statement_response.result().frame());
        }
    }

    std::unique_ptr<Results::Result>
    Cursor::execute(const std::string &language, const std::string &statement) {
        const std::string &nspace = connection.get_connection_properties().get_default_namespace();
        return execute(language, statement, nspace);
    }

    uint32_t Cursor::get_statement_id() const {
        return statement_id;
    }

    Connection &Cursor::get_connection() {
        return connection;
    }
} // namespace Connection
