#ifndef POLYPHENY_CPP_DRIVER_CURSOR_H
#define POLYPHENY_CPP_DRIVER_CURSOR_H

#include <string>
#include <vector>
#include <map>
#include "TypedValues.h"
#include "Connection.h"
#include "Result.h"

namespace Connection {

    class Cursor {
    private:
        const Connection& connection;
        int32_t statement_id;
        bool is_statement_id_set;
        void reset_statement();

        org::polypheny::prism::StatementResponse extract_statement_response(const org::polypheny::prism::Response& response);
    public:
        Cursor(const Connection& conn);

        Results::Result execute(const std::string& language, const std::string& statement, const std::string& nspace);
        Results::Result execute(const std::string& language, const std::string& statement);
        Results::Result execute(const std::string& language, const std::string& statement, const std::string& nspace, const std::vector<TypedValues>& parameters);
        Results::Result execute(const std::string& language, const std::string& statement, const std::vector<TypedValues>& parameters);
        Results::Result execute(const std::string& language, const std::string& statement, const std::string& nspace, const std::map<std::string, TypedValues>& parameters);
        Results::Result execute(const std::string& language, const std::string& statement, const std::map<std::string, TypedValues>& parameters);
    };

} // Connection

#endif //POLYPHENY_CPP_DRIVER_CURSOR_H
