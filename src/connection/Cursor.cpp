#include "src/connection/Cursor.h"
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
        if (is_prepared) {
            is_prepared = false;
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
        connection.get_prism_interface_client().execute_unparameterized_statement(nspace, language, statement,
                                                                                  callback);

        while (true) {
            org::polypheny::prism::Response response = callback->take_next();
            if (!response.has_statement_response()) {
                throw std::runtime_error("Received illegal response type on statement execution");
            }
            const org::polypheny::prism::StatementResponse &statement_response = response.statement_response();

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

    void Cursor::prepare(const std::string &language, const std::string &statement) {
        const std::string &nspace = connection.get_connection_properties().get_default_namespace();
        return prepare(language, statement, nspace);
    }

    void Cursor::prepare(const std::string &language, const std::string &statement, const std::string &nspace) {
        org::polypheny::prism::PreparedStatementSignature signature = connection.get_prism_interface_client().prepare_indexed_statement(
                nspace, language, statement);
        is_prepared = true;
    }

    std::unique_ptr<Results::Result> Cursor::execute_prepared(std::vector<Types::TypedValue> &params) {
        if (!is_prepared) {
            throw std::runtime_error("This operation requires a statement ot be prepared first");
        }
        org::polypheny::prism::StatementResult result = connection.get_prism_interface_client().execute_indexed_statement(
                statement_id, params, DEFAULT_FETCH_SIZE);
        if (!result.has_frame()) {
            return std::make_unique<Results::ScalarResult>(result.scalar());
        }
        return get_result_from_frame(result.frame());
    }

    std::unique_ptr<Results::Result>
    Cursor::execute_prepared(std::unordered_map<std::string, Types::TypedValue> &params) {
        if (!is_prepared) {
            throw std::runtime_error("This operation requires a statement ot be prepared first");
        }
        org::polypheny::prism::StatementResult result = connection.get_prism_interface_client().execute_named_statement(
                statement_id, params, DEFAULT_FETCH_SIZE);
        if (!result.has_frame()) {
            return std::make_unique<Results::ScalarResult>(result.scalar());
        }
        return get_result_from_frame(result.frame());
    }

    std::vector<uint64_t>
    Cursor::execute_prepared(std::vector<std::vector<Types::TypedValue>> &params_batch) {
        if (!is_prepared) {
            throw std::runtime_error("This operation requires a statement ot be prepared first");
        }
        org::polypheny::prism::StatementBatchResponse response = connection.get_prism_interface_client().execute_indexed_statement_batch(
                statement_id, params_batch);
        std::vector<uint64_t> update_counts;
        for (auto count: response.scalars()) {
            update_counts.push_back(count);
        }
        return update_counts;
    }

    std::vector<uint64_t>
    Cursor::execute(const std::string &language, const std::vector<std::string> &statements,
                    const std::string &nspace) {
        reset_statement();
        auto callback = std::make_shared<Communication::CallbackQueue>();
        connection.get_prism_interface_client().execute_unparameterized_statement_batch(nspace, language, statements,
                                                                                        callback);

        while (true) {
            org::polypheny::prism::Response response = callback->take_next();
            if (!response.has_statement_batch_response()) {
                throw std::runtime_error("Received illegal response type on statement batch execution");
            }
            const org::polypheny::prism::StatementBatchResponse &statement_batch_response = response.statement_batch_response();

            if (!is_statement_id_set) {
                statement_id = statement_batch_response.batch_id();
                is_statement_id_set = true;
            }

            if (!statement_batch_response.scalars_size() == 0) {
                continue;
            }

            callback->await_completion();
            std::vector<uint64_t> update_counts;
            for (auto count: statement_batch_response.scalars()) {
                update_counts.push_back(count);
            }
            return update_counts;
        }
    }

    std::vector<uint64_t> Cursor::execute(const std::string &language, const std::vector<std::string> &statements) {
        const std::string &nspace = connection.get_connection_properties().get_default_namespace();
        return execute(language, statements, nspace);
    }
} // namespace Connection
