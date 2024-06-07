#ifndef POLYPHENY_CPP_DRIVER_CURSOR_H
#define POLYPHENY_CPP_DRIVER_CURSOR_H

#include <string>
#include <functional>
#include <memory>
#include <stdexcept>
#include "src/types/TypedValue.h"
#include "protointerface.pb.h"
#include "statement_responses.pb.h"
#include "src/results/Result.h"
#include "src/results/RelationalResult.h"
#include "src/results/DocumentResult.h"
#include "src/results/GraphResult.h"
#include "src/results/ScalarResult.h"

namespace Connection {
    // forward declaration, include in .cpp
    class Connection;

    class Cursor {
    private:
        uint32_t statement_id{};
        bool is_statement_id_set{};
        Connection &connection;

        void reset_statement();

        std::unique_ptr<Results::Result>
        get_result_from_frame(const org::polypheny::prism::Frame &frame);

    protected:
        [[nodiscard]] uint32_t get_statement_id() const;

        Connection& get_connection();

        friend class Results::RelationalResult;
        friend class Results::DocumentResult;
        friend class Results::GraphResult;

    public:
        explicit Cursor(Connection &connection);

        Cursor(const Cursor &other);

        Cursor &operator=(const Cursor &other);

        ~Cursor();

        std::unique_ptr<Results::Result>
        execute(const std::string &language, const std::string &statement, const std::string &nspace);

        std::unique_ptr<Results::Result> execute(const std::string &language, const std::string &statement);
    };

} // namespace Connection

#endif // POLYPHENY_CPP_DRIVER_CURSOR_H
