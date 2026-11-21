#include <catch2/catch_all.hpp>
#include <PolyphenyCPPDriver.h>
#include <condition_variable>
#include <functional>
#include <queue>

namespace {

    namespace prism = org::polypheny::prism;

    class GraphTestTransport final : public Transport::Transport {
    public:
        using Handler = std::function<prism::Response(const prism::Request &)>;
        explicit GraphTestTransport(Handler handler) : handler(std::move(handler)) {}
        void connect() override {}
        void send_message(const std::string &message) override {
            prism::Request request;
            REQUIRE(request.ParseFromString(message));
            prism::Response response;
            if (request.has_connection_request()) {
                auto *connection = response.mutable_connection_response();
                connection->set_major_api_version(POLYPHENY_PRISM_STREAMING ? 5 : 2);
                connection->set_minor_api_version(POLYPHENY_PRISM_STREAMING ? 1 : 3);
                connection->set_is_compatible(true);
            } else if (request.has_disconnect_request()) {
                response.mutable_disconnect_response();
            } else {
                response = handler(request);
            }
            response.set_id(request.id());
            response.set_last(true);
            {
                std::lock_guard<std::mutex> lock(mutex);
                responses.push(response.SerializeAsString());
            }
            ready.notify_one();
        }
        std::string receive_message() override {
            std::unique_lock<std::mutex> lock(mutex);
            ready.wait(lock, [&] { return closed || !responses.empty(); });
            if (responses.empty()) {
                throw std::runtime_error("Test transport closed");
            }
            auto response = std::move(responses.front());
            responses.pop();
            return response;
        }
        void shutdown() noexcept override {
            {
                std::lock_guard<std::mutex> lock(mutex);
                closed = true;
            }
            ready.notify_all();
        }

    private:
        Handler handler;
        std::mutex mutex;
        std::condition_variable ready;
        std::queue<std::string> responses;
        bool closed = false;
    };

    prism::Response statement_response(prism::Frame frame, int id = 7) {
        prism::Response response;
        auto *statement = response.mutable_statement_response();
        statement->set_statement_id(id);
        *statement->mutable_result()->mutable_frame() = std::move(frame);
        return response;
    }

    prism::Frame graph_page(const std::vector<std::string> &ids, bool last) {
        prism::Frame frame;
        frame.set_is_last(last);
        auto *graph = frame.mutable_graph_frame();
        for (const auto &id : ids) {
            graph->add_element()->mutable_node()->set_id(id);
        }
        return frame;
    }

    prism::Frame relational_page(int index, bool last) {
        prism::Frame frame;
        frame.set_is_last(last);
        auto *relational = frame.mutable_relational_frame();
        auto *meta = relational->add_column_meta();
        meta->set_column_label("name");
        meta->set_column_index(0);
        meta->mutable_type_meta()->set_proto_value_type(prism::VARCHAR);
        relational->add_rows()->add_values()->mutable_string()->set_string("person-" + std::to_string(index));
        return frame;
    }

}

TEST_CASE("Graph iteration follows pages and skips empty nonfinal graph frames", "[graph]") {
    int fetched = 0;
    int closed = 0;
    auto transport = std::make_unique<GraphTestTransport>([&](const prism::Request &request) {
        prism::Response response;
        if (request.has_execute_unparameterized_statement_request()) {
            CHECK(request.execute_unparameterized_statement_request().fetch_size() == 1);
            return statement_response(graph_page({}, false));
        }
        if (request.has_fetch_request()) {
            CHECK(request.fetch_request().statement_id() == 7);
            CHECK(request.fetch_request().fetch_size() == 1);
            ++fetched;
            *response.mutable_frame() = fetched == 1 ? graph_page({}, false)
                    : graph_page({"node-" + std::to_string(fetched)}, fetched == 4);
        } else if (request.has_close_result_request()) {
            CHECK(request.close_result_request().statement_id() == 7);
            ++closed;
            response.mutable_close_result_response();
        }
        return response;
    });
    Connection::ConnectionProperties properties;
    Connection::Connection connection(properties, std::move(transport));
    Connection::Cursor cursor(connection);
    cursor.set_fetch_size(1);
    CHECK(cursor.get_fetch_size() == 1);
    CHECK_THROWS_AS(cursor.set_fetch_size(0), std::invalid_argument);
    auto result = cursor.execute("cypher", "MATCH (n) RETURN n");
    auto *graph = result->unwrap<Results::GraphResult>();
    std::vector<std::string> ids;
    for (auto element : *graph) {
        ids.push_back(element->get_id());
    }
    CHECK(ids == std::vector<std::string>{"node-2", "node-3", "node-4"});
    CHECK(fetched == 4);
    result->close();
    result->close();
    CHECK(closed == 1);
    CHECK_THROWS_AS(graph->begin(), std::runtime_error);
}

TEST_CASE("Graph-only empty results expose an empty range and unknown elements fail", "[graph]") {
    for (bool unknown : {false, true}) {
        auto transport = std::make_unique<GraphTestTransport>([&](const prism::Request &request) {
            prism::Response response;
            if (request.has_execute_unparameterized_statement_request()) {
                auto frame = graph_page({}, true);
                if (unknown) {
                    frame.mutable_graph_frame()->add_element();
                }
                return statement_response(frame);
            }
            if (request.has_close_result_request()) {
                response.mutable_close_result_response();
            }
            return response;
        });
        Connection::ConnectionProperties properties;
        Connection::Connection connection(properties, std::move(transport));
        Connection::Cursor cursor(connection);
        if (unknown) {
            CHECK_THROWS_AS(cursor.execute("cypher", "MATCH (n) RETURN n"), std::runtime_error);
        } else {
            auto result = cursor.execute("cypher", "MATCH (n) RETURN n");
            auto *graph = result->unwrap<Results::GraphResult>();
            CHECK(graph->begin() == graph->end());
        }
    }
}

TEST_CASE("Relational projections use a stable end sentinel across all pages", "[graph]") {
    int fetched = 0;
    auto transport = std::make_unique<GraphTestTransport>([&](const prism::Request &request) {
        prism::Response response;
        if (request.has_execute_unparameterized_statement_request()) {
            return statement_response(relational_page(0, false));
        }
        if (request.has_fetch_request()) {
            ++fetched;
            *response.mutable_frame() = relational_page(fetched, fetched == 3);
        } else if (request.has_close_result_request()) {
            response.mutable_close_result_response();
        }
        return response;
    });
    Connection::ConnectionProperties properties;
    Connection::Connection connection(properties, std::move(transport));
    Connection::Cursor cursor(connection);
    auto result = cursor.execute("cypher", "MATCH (n) RETURN n.name AS name");
    auto *rows = result->unwrap<Results::RelationalResult>();
    std::vector<std::string> names;
    for (auto &row : *rows) {
        names.push_back(row["name"].as_string());
    }
    CHECK(names == std::vector<std::string>{"person-0", "person-1", "person-2", "person-3"});
    CHECK(rows->get_metadata()->get_column_meta(0).get_column_label() == "name");
}

TEST_CASE("A graph fetch returning another frame type raises a driver error", "[graph]") {
    int closed = 0;
    auto transport = std::make_unique<GraphTestTransport>([&](const prism::Request &request) {
        prism::Response response;
        if (request.has_execute_unparameterized_statement_request()) {
            return statement_response(graph_page({"one"}, false));
        }
        if (request.has_fetch_request()) {
            *response.mutable_frame() = relational_page(1, true);
        } else if (request.has_close_result_request()) {
            ++closed;
            response.mutable_close_result_response();
        }
        return response;
    });
    Connection::ConnectionProperties properties;
    Connection::Connection connection(properties, std::move(transport));
    Connection::Cursor cursor(connection);
    auto result = cursor.execute("cypher", "MATCH (n) RETURN n");
    auto iterator = result->unwrap<Results::GraphResult>()->begin();
    CHECK_THROWS_AS(++iterator, std::runtime_error);
    CHECK(closed == 1);
    result->close();
    CHECK(closed == 1);
}

TEST_CASE("Failed graph frame decoding closes its result and permits recovery", "[graph]") {
    int closed = 0;
    int statements = 0;
    auto transport = std::make_unique<GraphTestTransport>([&](const prism::Request &request) {
        prism::Response response;
        if (request.has_execute_unparameterized_statement_request()) {
            return statement_response(graph_page({"one"}, ++statements > 1));
        }
        if (request.has_fetch_request()) {
            auto frame = graph_page({}, true);
            frame.mutable_graph_frame()->add_element();
            *response.mutable_frame() = std::move(frame);
        } else if (request.has_close_result_request()) {
            ++closed;
            response.mutable_close_result_response();
        } else if (request.has_close_statement_request()) {
            response.mutable_close_statement_response();
        }
        return response;
    });
    Connection::ConnectionProperties properties;
    Connection::Connection connection(properties, std::move(transport));
    Connection::Cursor cursor(connection);
    auto result = cursor.execute("cypher", "MATCH (n) RETURN n");
    auto iterator = result->unwrap<Results::GraphResult>()->begin();
    CHECK_THROWS_AS(++iterator, std::runtime_error);
    CHECK(closed == 1);
    auto recovered = cursor.execute("cypher", "MATCH (n) RETURN n");
    CHECK(recovered->unwrap<Results::GraphResult>()->begin() != recovered->unwrap<Results::GraphResult>()->end());
}

TEST_CASE("Unknown initial frames close the acknowledged statement before recovery", "[graph]") {
    int statements = 0;
    std::vector<uint32_t> closed;
    auto transport = std::make_unique<GraphTestTransport>([&](const prism::Request &request) {
        prism::Response response;
        if (request.has_execute_unparameterized_statement_request()) {
            ++statements;
            return statement_response(statements == 1 ? prism::Frame{} : graph_page({}, true), statements);
        }
        if (request.has_close_statement_request()) {
            closed.push_back(request.close_statement_request().statement_id());
            response.mutable_close_statement_response();
        } else if (request.has_close_result_request()) {
            response.mutable_close_result_response();
        }
        return response;
    });
    Connection::ConnectionProperties properties;
    Connection::Connection connection(properties, std::move(transport));
    Connection::Cursor cursor(connection);
    CHECK_THROWS_AS(cursor.execute("cypher", "MATCH (n) RETURN n"), std::runtime_error);
    CHECK(closed == std::vector<uint32_t>{1});
    auto recovered = cursor.execute("cypher", "MATCH (n) RETURN n");
    CHECK(recovered->unwrap<Results::GraphResult>()->begin() == recovered->unwrap<Results::GraphResult>()->end());
}

TEST_CASE("Result cleanup keeps its original statement and survives a destroyed cursor", "[graph]") {
    std::vector<uint32_t> closed;
    auto transport = std::make_unique<GraphTestTransport>([&](const prism::Request &request) {
        prism::Response response;
        if (request.has_execute_unparameterized_statement_request()) {
            return statement_response(graph_page({"one"}, false), 42);
        }
        if (request.has_close_result_request()) {
            closed.push_back(request.close_result_request().statement_id());
            response.mutable_close_result_response();
        }
        return response;
    });
    Connection::ConnectionProperties properties;
    Connection::Connection connection(properties, std::move(transport));
    std::unique_ptr<Results::Result> result;
    {
        Connection::Cursor cursor(connection);
        result = cursor.execute("cypher", "MATCH (n) RETURN n");
    }
    result->close();
    result.reset();
    CHECK(closed == std::vector<uint32_t>{42});
}

TEST_CASE("Malformed statement responses and an empty batch cannot hang the cursor", "[graph]") {
    auto transport = std::make_unique<GraphTestTransport>([](const prism::Request &request) {
        prism::Response response;
        if (request.has_execute_unparameterized_statement_request()) {
            response.mutable_statement_response()->set_statement_id(2);
        } else if (request.has_execute_unparameterized_statement_batch_request()) {
            response.mutable_statement_batch_response()->set_batch_id(3);
        } else if (request.has_close_statement_request()) {
            response.mutable_close_statement_response();
        }
        return response;
    });
    Connection::ConnectionProperties properties;
    Connection::Connection connection(properties, std::move(transport));
    Connection::Cursor cursor(connection);
    CHECK_THROWS_AS(cursor.execute("cypher", "MATCH (n) RETURN n"), std::runtime_error);
    CHECK(cursor.execute("cypher", std::vector<std::string>{}).empty());
}
