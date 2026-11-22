#include "src/connection/Connection.h"

#include <iostream>

#include "src/connection/Cursor.h"


namespace Connection {

    // INFO: the statement_id set here is a placeholder value ignored as is_statement_id_set == false
    Cursor::Cursor(Connection &connection)
            : statement_id(0),
              is_statement_id_set(false),
              is_prepared(false),
              connection(connection),
              streaming_index(std::make_shared<Streaming::StreamingIndex>(connection.get_prism_interface_client())) {
    }

    Cursor::Cursor(const Cursor &other)
            : Cursor(other.connection) {
        fetch_size = other.fetch_size;
    }

    void Cursor::close_active_result() {
        if (auto result = active_result.lock()) {
            result->close();
        }
        active_result.reset();
    }

    void Cursor::reset_statement() {
        close_active_result();
        if (is_statement_id_set) {
            statement_state->close();
            statement_state.reset();
            is_statement_id_set = false;
            streaming_index->update(0, false);
        }
        if (is_prepared) {
            is_prepared = false;
        }
    }

    void Cursor::discard_failed_statement() noexcept {
        try {
            reset_statement();
        } catch (...) {
            is_statement_id_set = false;
            is_prepared = false;
            statement_state.reset();
            streaming_index->update(0, false);
        }
    }

    std::unique_ptr<Results::Result> Cursor::get_result_from_frame(const org::polypheny::prism::Frame &frame) {
        bool is_last = frame.is_last();
        std::unique_ptr<Results::Result> result;
        switch (frame.result_case()) {
            case org::polypheny::prism::Frame::ResultCase::kRelationalFrame:
                result = std::make_unique<Results::RelationalResult>(frame.relational_frame(), is_last, this);
                break;
            case org::polypheny::prism::Frame::ResultCase::kDocumentFrame:
                result = std::make_unique<Results::DocumentResult>(frame.document_frame(), is_last, this);
                break;
            case org::polypheny::prism::Frame::ResultCase::kGraphFrame:
                result = std::make_unique<Results::GraphResult>(frame.graph_frame(), is_last, this);
                break;
            default:
                throw std::runtime_error("Frame of illegal result type encountered");
        }
        result->state->statement = statement_state;
        active_result = result->state;
        return result;
    }

    std::unique_ptr<Results::Result>
    Cursor::execute(const std::string &language, const std::string &statement, const std::string &nspace) {
        reset_statement();
        auto callback = std::make_shared<Communication::CallbackQueue>();
        connection.get_prism_interface_client()->execute_unparameterized_statement(nspace, language, statement,
                                                                                   callback, fetch_size);

        try {
            while (true) {
            org::polypheny::prism::Response response = callback->take_next();
            if (!response.has_statement_response()) {
                throw std::runtime_error("Received illegal response type on statement execution");
            }
            const org::polypheny::prism::StatementResponse &statement_response = response.statement_response();

            if (!is_statement_id_set) {
                statement_id = statement_response.statement_id();
                streaming_index->update(statement_id, true);
                is_statement_id_set = true;
                statement_state = std::make_shared<Communication::StatementState>(
                        connection.get_prism_interface_client(), statement_id);
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
        } catch (...) {
            discard_failed_statement();
            throw;
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

    void Cursor::prepare_indexed(const std::string &language, const std::string &statement) {
        const std::string &nspace = connection.get_connection_properties().get_default_namespace();
        return prepare_indexed(language, statement, nspace);
    }

    void Cursor::prepare_indexed(const std::string &language, const std::string &statement, const std::string &nspace) {
        reset_statement();
        org::polypheny::prism::PreparedStatementSignature signature = connection.get_prism_interface_client()->prepare_indexed_statement(
                nspace, language, statement);
        statement_id = signature.statement_id();
        is_statement_id_set = true;
        statement_state = std::make_shared<Communication::StatementState>(
                connection.get_prism_interface_client(), statement_id);
        streaming_index->update(statement_id, true);
        is_prepared = true;
    }

    void Cursor::prepare_named(const std::string &language, const std::string &statement) {
        const std::string &nspace = connection.get_connection_properties().get_default_namespace();
        return prepare_named(language, statement, nspace);
    }

    void Cursor::prepare_named(const std::string &language, const std::string &statement, const std::string &nspace) {
        reset_statement();
        org::polypheny::prism::PreparedStatementSignature signature = connection.get_prism_interface_client()->prepare_named_statement(
                nspace, language, statement);
        statement_id = signature.statement_id();
        is_statement_id_set = true;
        statement_state = std::make_shared<Communication::StatementState>(
                connection.get_prism_interface_client(), statement_id);
        streaming_index->update(statement_id, true);
        is_prepared = true;
    }

    std::unique_ptr<Results::Result> Cursor::execute_prepared(std::vector<Types::TypedValue> &params) {
        if (!is_prepared) {
            throw std::runtime_error("This operation requires a statement to be prepared first");
        }
        close_active_result();
        org::polypheny::prism::StatementResult result = connection.get_prism_interface_client()->execute_indexed_statement(
                statement_id, params, fetch_size, streaming_index);
        if (!result.has_frame()) {
            return std::make_unique<Results::ScalarResult>(result.scalar());
        }
        return get_result_from_frame(result.frame());
    }

    std::unique_ptr<Results::Result>
    Cursor::execute_prepared(std::unordered_map<std::string, Types::TypedValue> &params) {
        if (!is_prepared) {
            throw std::runtime_error("This operation requires a statement to be prepared first");
        }
        close_active_result();
        org::polypheny::prism::StatementResult result = connection.get_prism_interface_client()->execute_named_statement(
                statement_id, params, fetch_size, streaming_index);
        if (!result.has_frame()) {
            return std::make_unique<Results::ScalarResult>(result.scalar());
        }
        return get_result_from_frame(result.frame());
    }

    std::vector<uint64_t>
    Cursor::execute_prepared(std::vector<std::vector<Types::TypedValue>> &params_batch) {
        if (!is_prepared) {
            throw std::runtime_error("This operation requires a statement to be prepared first");
        }
        close_active_result();
        org::polypheny::prism::StatementBatchResponse response = connection.get_prism_interface_client()->execute_indexed_statement_batch(
                statement_id, params_batch, streaming_index);
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
        connection.get_prism_interface_client()->execute_unparameterized_statement_batch(nspace, language, statements,
                                                                                         callback);

        try {
            while (true) {
                org::polypheny::prism::Response response = callback->take_next();
                if (!response.has_statement_batch_response()) {
                    throw std::runtime_error("Received illegal response type on statement batch execution");
                }
                const auto &statement_batch_response = response.statement_batch_response();

                if (!is_statement_id_set) {
                    statement_id = statement_batch_response.batch_id();
                    streaming_index->update(statement_id, true);
                    is_statement_id_set = true;
                    statement_state = std::make_shared<Communication::StatementState>(
                            connection.get_prism_interface_client(), statement_id);
                }

                if (statement_batch_response.scalars_size() == 0 && !response.last()) {
                    continue;
                }

                callback->await_completion();
                std::vector<uint64_t> update_counts;
                for (auto count: statement_batch_response.scalars()) {
                    update_counts.push_back(count);
                }
                return update_counts;
            }
        } catch (...) {
            discard_failed_statement();
            throw;
        }
    }

    std::vector<uint64_t> Cursor::execute(const std::string &language, const std::vector<std::string> &statements) {
        const std::string &nspace = connection.get_connection_properties().get_default_namespace();
        return execute(language, statements, nspace);
    }

    Cursor::~Cursor() = default;

    void Cursor::set_fetch_size(uint32_t size) {
        if (size == 0) {
            throw std::invalid_argument("Fetch size must be greater than zero");
        }
        fetch_size = size;
    }

    uint32_t Cursor::get_fetch_size() const noexcept {
        return fetch_size;
    }
} // namespace Connection
