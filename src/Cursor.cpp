#include "Cursor.h"
#include <iostream>

namespace Connection {

    Cursor::Cursor(Connection *connection) : connection(connection), statement_id(0), is_statement_id_set(false) {
    }

    void Cursor::reset_statement() {
        if (is_statement_id_set) {
            is_statement_id_set = false;
        }
    }

    Results::Result
    Cursor::execute(const std::string &language, const std::string &statement, const std::string &nspace) {
        reset_statement();
        Communication::CallbackQueue<org::polypheny::prism::StatementResponse> callback(response_extractor);
        connection.get_prism_interface_client().execute_unparameterized_statement(nspace, language, statement,
                                                                                  callback);
        while (true) {
            org::polypheny::prism::StatementResponse statement_response = callback.take_next();
            if (!is_statement_id_set) {
                statement_id = statement_response.statement_id();
                is_statement_id_set = true;
            }
            if (!statement_response.has_result()) {
                continue;
            }
            callback.await_completion()
            if (!statement_response.result.has_frame()) {
                return ScalarResult(statement_response.result().scalar());
            }
            return get_result_from_frame(statement_response.result().frame());
        }
    }

    Results::Result Cursor::execute(const std::string &language, const std::string &statement) {
        const std::string &nspace = connection.get_connection_properties().get_default_namespace();
        return execute(language, statement, nspace);
    }

    Results::Result
    Cursor::execute(const std::string &language, const std::string &statement, const std::string &nspace,
                    const std::vector<TypedValues> &params) {
        // TODO: Implement the logic to execute the parameterized statement with namespace
        throw std::runtime_error("Not implemented yet!");
    }

    Results::Result
    Cursor::execute(const std::string &language, const std::string &statement, const std::vector<TypedValues> &params) {
        // TODO: Implement the logic to execute the parameterized statement without namespace
        throw std::runtime_error("Not implemented yet!");
    }

    Results::Result
    Cursor::execute(const std::string &language, const std::string &statement, const std::string &nspace,
                    const std::map<std::string, TypedValues> &params) {
        // TODO: Implement the logic to execute the parameterized statement with namespace
        throw std::runtime_error("Not implemented yet!");
    }

    Results::Result Cursor::execute(const std::string &language, const std::string &statement,
                                    const std::map<std::string, TypedValues> &params) {
        // TODO: Implement the logic to execute the parameterized statement without namespace
        throw std::runtime_error("Not implemented yet!");
    }

}
