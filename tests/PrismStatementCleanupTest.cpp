#include <catch2/catch_all.hpp>
#include <PolyphenyCPPDriver.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "PrismConfiguration.h"
#include "ScriptedTransport.h"

TEST_CASE("Prism closes an acknowledged statement when execution fails", "[prism][cleanup]") {
    using TestSupport::ScriptedTransport;
    using Request = ScriptedTransport::Request;
    using Response = ScriptedTransport::Response;
    using ReplySequence = ScriptedTransport::ReplySequence;

    const bool batch = GENERATE(false, true);
    const bool close_fails = GENERATE(false, true);
    CAPTURE(batch, close_fails);
    constexpr std::uint32_t failed_id = 42;
    constexpr std::uint32_t recovered_id = 43;
    const std::string failing_statement = "INSERT INTO items VALUES (1)";
    const std::string recovery_statement = "DELETE FROM items";
    const std::string statement_error = "Duplicate value";
    std::vector<std::uint32_t> closed_ids;
    bool rolled_back = false;

    auto transport = std::make_unique<ScriptedTransport>([&](const Request &request) -> ReplySequence {
        Response reply;
        reply.set_last(true);
        if (request.has_close_statement_request()) {
            const auto id = request.close_statement_request().statement_id();
            closed_ids.push_back(id);
            CHECK((id == failed_id || id == recovered_id));
            if (id == failed_id && close_fails) {
                reply.mutable_error_response()->set_message("Failed to close statement");
            } else {
                reply.mutable_close_statement_response();
            }
            return {reply};
        }
        if (request.has_rollback_request()) {
            rolled_back = true;
            reply.mutable_rollback_response();
            return {reply};
        }
        if (request.has_execute_unparameterized_statement_request()
                && request.execute_unparameterized_statement_request().statement() == recovery_statement) {
            CHECK(rolled_back);
            auto *result = reply.mutable_statement_response();
            result->set_statement_id(recovered_id);
            result->mutable_result()->set_scalar(1);
            return {reply};
        }

        Response acknowledgement;
        if (batch) {
            REQUIRE(request.has_execute_unparameterized_statement_batch_request());
            const auto &statements = request.execute_unparameterized_statement_batch_request().statements();
            REQUIRE(statements.size() == 1);
            CHECK(statements.Get(0).statement() == failing_statement);
            acknowledgement.mutable_statement_batch_response()->set_batch_id(failed_id);
        } else {
            REQUIRE(request.has_execute_unparameterized_statement_request());
            CHECK(request.execute_unparameterized_statement_request().statement() == failing_statement);
            acknowledgement.mutable_statement_response()->set_statement_id(failed_id);
        }

        auto *error = reply.mutable_error_response();
        error->set_message(statement_error);
#if !POLYPHENY_PRISM_STREAMING
        error->mutable_details()->set_state("23505");
        error->mutable_details()->set_error_code(0);
#endif
        return {acknowledgement, reply};
    });

    Connection::ConnectionProperties properties;
    properties.set_is_auto_commit(false);
    Connection::Connection connection(properties, std::move(transport));
    {
        Connection::Cursor cursor(connection);
        try {
            if (batch) {
                cursor.execute("sql", std::vector<std::string>{failing_statement});
            } else {
                cursor.execute("sql", failing_statement);
            }
            FAIL("Expected a statement error");
        } catch (const Errors::ServerError &error) {
            CHECK(error.what() == statement_error);
#if POLYPHENY_PRISM_STREAMING
            CHECK_FALSE(error.get_sql_state().has_value());
            CHECK_FALSE(error.get_error_code().has_value());
#else
            CHECK(error.get_sql_state() == "23505");
            CHECK(error.get_error_code() == 0);
#endif
        }

        REQUIRE(closed_ids == std::vector<std::uint32_t>{failed_id});
        REQUIRE_NOTHROW(connection.rollback());
        auto result = cursor.execute("sql", recovery_statement);
        CHECK(result->unwrap<Results::ScalarResult>()->get_scalar() == 1);
    }
    CHECK(closed_ids == std::vector<std::uint32_t>{failed_id, recovered_id});
}
