#ifndef POLYPHENY_CPP_DRIVER_CURSOR_H
#define POLYPHENY_CPP_DRIVER_CURSOR_H

#include <string>
#include <functional>
#include <memory>
#include <stdexcept>
#include "TypedValue.h"
#include "Connection.h"
#include "protointerface.pb.h"
#include "statement_responses.pb.h"
#include "Result.h"
#include "RelationalResult.h"
#include "DocumentResult.h"
#include "GraphResult.h"
#include "ScalarResult.h"

namespace Connection {

    class Cursor {
    private:
        int32_t statement_id;
        bool is_statement_id_set;
        Connection &connection;

        void reset_statement();

        static std::unique_ptr<Results::Result>
        get_result_from_frame(const org::polypheny::prism::Frame &frame);

    public:
        Cursor(Connection &connection);

        std::unique_ptr<Results::Result>
        execute(const std::string &language, const std::string &statement, const std::string &nspace);

        std::unique_ptr<Results::Result> execute(const std::string &language, const std::string &statement);
    };

} // namespace Connection

#endif // POLYPHENY_CPP_DRIVER_CURSOR_H
