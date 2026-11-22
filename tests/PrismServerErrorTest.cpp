#include <catch2/catch_all.hpp>
#include <PolyphenyCPPDriver.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "PrismConfiguration.h"
#include "ScriptedTransport.h"

namespace {

    using TestSupport::ScriptedTransport;
    using Request = ScriptedTransport::Request;
    using Response = ScriptedTransport::Response;

    // A state-like token in the message must not influence the decoded SQLSTATE.
    constexpr char legacy_message[] = "  Server rejected the request (42000)\n";
    constexpr uint32_t statement_id = 42;

    void send_error(Response &response, [[maybe_unused]] const std::optional<std::string> &state = {},
                    [[maybe_unused]] std::optional<int32_t> code = {}) {
        auto *error = response.mutable_error_response();
        error->set_message(legacy_message);
#if !POLYPHENY_PRISM_STREAMING
        if (state || code) {
            auto *details = error->mutable_details();
            details->set_message("A different message in the detail field");
            if (state) {
                details->set_state(*state);
            }
            if (code) {
                details->set_error_code(*code);
            }
        }
#endif
    }

    void require_server_error(const std::function<void()> &operation, std::optional<std::string> state = {},
                              std::optional<int32_t> code = {}) {
#if POLYPHENY_PRISM_STREAMING
        state.reset();
        code.reset();
#endif
        try {
            operation();
            FAIL("Expected a ServerError");
        } catch (const Errors::ServerError &error) {
            CHECK(std::string(error.what()) == legacy_message);
            CHECK(error.get_sql_state() == state);
            CHECK(error.get_error_code() == code);
        }
    }

    void require_protocol_error(const std::function<void()> &operation, const char *message) {
        try {
            operation();
            FAIL("Expected a Prism protocol error");
        } catch (const Errors::ServerError &error) {
            FAIL("Received ServerError with SQLSTATE " << error.get_sql_state().value_or("<absent>"));
        } catch (const std::runtime_error &error) {
            CHECK(std::string(error.what()) == message);
        }
    }

}

TEST_CASE("Prism login errors preserve the legacy message and server details", "[prism]") {
    Connection::ConnectionProperties properties;
    properties.set_username("missing-user");
    auto transport = std::make_unique<ScriptedTransport>(nullptr, [](const Request &request, Response &response) {
        CHECK(request.connection_request().username() == "missing-user");
        send_error(response, "28000", 0);
    });

    require_server_error([&] { Connection::Connection connection(properties, std::move(transport)); }, "28000", 0);
}

TEST_CASE("Direct Prism statement errors reach the cursor and permit rollback", "[prism]") {
    Connection::ConnectionProperties properties;
    properties.set_is_auto_commit(false);
    auto transport = std::make_unique<ScriptedTransport>([](const Request &request, Response &response) {
        if (request.has_rollback_request()) {
            response.mutable_rollback_response();
        } else if (request.has_close_statement_request()) {
            response.mutable_close_statement_response();
        } else {
            REQUIRE(request.has_execute_unparameterized_statement_request());
            if (request.execute_unparameterized_statement_request().statement() == "INSERT INTO items VALUES (1)") {
                send_error(response, "23505", 0);
            } else {
                auto *result = response.mutable_statement_response();
                result->set_statement_id(statement_id);
                result->mutable_result()->set_scalar(1);
            }
        }
    });
    Connection::Connection connection(properties, std::move(transport));
    Connection::Cursor cursor(connection);

    require_server_error([&] { cursor.execute("sql", "INSERT INTO items VALUES (1)"); }, "23505", 0);
    REQUIRE_NOTHROW(connection.rollback());
    auto result = cursor.execute("sql", "DELETE FROM items");
    CHECK(result->unwrap<Results::ScalarResult>()->get_scalar() == 1);
}

TEST_CASE("Prism prepared statement RPCs propagate server details", "[prism]") {
    Connection::ConnectionProperties properties;
    auto transport = std::make_unique<ScriptedTransport>([](const Request &request, Response &response) {
        if (request.has_prepare_indexed_statement_request()) {
            if (request.prepare_indexed_statement_request().statement() == "SELECT ?") {
                response.mutable_prepared_statement_signature()->set_statement_id(statement_id);
            } else {
                send_error(response, "42601");
            }
        } else if (request.has_close_statement_request()) {
            response.mutable_close_statement_response();
        } else {
            REQUIRE(request.has_execute_indexed_statement_request());
            CHECK(request.execute_indexed_statement_request().statement_id() == statement_id);
            send_error(response, {}, -17);
        }
    });
    Connection::Connection connection(properties, std::move(transport));
    Connection::Cursor cursor(connection);

    SECTION("Prepare fails with a state and no code") {
        require_server_error([&] { cursor.prepare_indexed("sql", "SELECT FROM"); }, "42601");
    }

    SECTION("Execute fails with a code and no state") {
        cursor.prepare_indexed("sql", "SELECT ?");
        std::vector<Types::TypedValue> parameters;
        require_server_error([&] { cursor.execute_prepared(parameters); }, {}, -17);
    }
}

TEST_CASE("Prism fetch errors reach the result iterator", "[prism]") {
    Connection::ConnectionProperties properties;
    auto transport = std::make_unique<ScriptedTransport>([](const Request &request, Response &response) {
        if (request.has_execute_unparameterized_statement_request()) {
            auto *result = response.mutable_statement_response();
            result->set_statement_id(statement_id);
            result->mutable_result()->mutable_frame()->mutable_relational_frame();
        } else if (request.has_close_result_request()) {
            response.mutable_close_result_response();
        } else if (request.has_close_statement_request()) {
            response.mutable_close_statement_response();
        } else {
            REQUIRE(request.has_fetch_request());
            CHECK(request.fetch_request().statement_id() == statement_id);
            send_error(response, "XX000", 7);
        }
    });
    Connection::Connection connection(properties, std::move(transport));
    Connection::Cursor cursor(connection);
    auto result = cursor.execute("sql", "SELECT * FROM items");
    auto *rows = result->unwrap<Results::RelationalResult>();

    require_server_error([&] { rows->begin(); }, "XX000", 7);
}

TEST_CASE("Prism errors without structured fields do not infer metadata from the message", "[prism]") {
    bool empty_details = false;
    SECTION("Message-only response") {}
#if !POLYPHENY_PRISM_STREAMING
    SECTION("Details without state or error code") {
        empty_details = true;
    }
#endif
    Connection::ConnectionProperties properties;
    auto transport = std::make_unique<ScriptedTransport>([empty_details](const Request &request, Response &response) {
        REQUIRE(request.has_execute_unparameterized_statement_request());
        send_error(response);
#if !POLYPHENY_PRISM_STREAMING
        if (empty_details) {
            response.mutable_error_response()->mutable_details();
        }
#else
        (void) empty_details;
#endif
    });
    Connection::Connection connection(properties, std::move(transport));
    Connection::Cursor cursor(connection);
    require_server_error([&] { cursor.execute("sql", "SELECT FROM"); });
}

TEST_CASE("An invalid Prism response ID remains a protocol error", "[prism]") {
    Connection::ConnectionProperties properties;
    auto transport = std::make_unique<ScriptedTransport>([](const Request &request, Response &response) {
        REQUIRE(request.has_execute_unparameterized_statement_request());
        send_error(response, "XX000", 7);
        response.set_id(0);
    });
    Connection::Connection connection(properties, std::move(transport));
    Connection::Cursor cursor(connection);

    require_protocol_error([&] { cursor.execute("sql", "SELECT * FROM items"); }, "Invalid message id");
}

TEST_CASE("A non-terminal Prism error response is a protocol error", "[prism][framing]") {
    Connection::ConnectionProperties properties;
    auto transport = std::make_unique<ScriptedTransport>([](const Request &request, Response &response) {
        REQUIRE((request.has_execute_unparameterized_statement_request()
                 || request.has_prepare_indexed_statement_request()));
        send_error(response, "23505", 0);
        response.set_last(false);
    });
    Connection::Connection connection(properties, std::move(transport));
    Connection::Cursor cursor(connection);

    SECTION("Direct execution uses the callback queue") {
        require_protocol_error([&] { cursor.execute("sql", "INSERT INTO items VALUES (1)"); },
                               "Non-terminal Prism error response");
        REQUIRE_THROWS_WITH(connection.rollback(), "Connection is closed");
    }

    SECTION("Preparation uses the synchronous response path") {
        require_protocol_error([&] { cursor.prepare_indexed("sql", "INSERT INTO items VALUES (?)"); },
                               "Non-terminal Prism error response");
        REQUIRE_THROWS_WITH(connection.rollback(), "Connection is closed");
    }
}
