#include <catch2/catch_all.hpp>
#include <PolyphenyCPPDriver.h>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <set>

#if !POLYPHENY_PRISM_STREAMING
namespace {

    std::string environment(const char *name, const char *fallback) {
        const char *value = std::getenv(name);
        return value ? value : fallback;
    }

    std::unique_ptr<Connection::Connection> open_connection(bool autocommit = true) {
        Connection::ConnectionProperties properties;
        properties.set_username(environment("POLYPHENY_PRISM_USER", "pa"));
        properties.set_password(environment("POLYPHENY_PRISM_PASSWORD", ""));
        properties.set_is_auto_commit(autocommit);
        auto port = std::stoul(environment("POLYPHENY_PRISM_PORT", "20590"));
        auto transport = std::make_unique<Transport::PlainTcpTransport>(
                environment("POLYPHENY_PRISM_HOST", "127.0.0.1"), static_cast<uint16_t>(port));
        return std::make_unique<Connection::Connection>(properties, std::move(transport));
    }

    class GraphFixture {
    public:
        std::unique_ptr<Connection::Connection> connection = open_connection();
        Connection::Cursor cursor{*connection};
        std::string name;

        GraphFixture() {
            static std::atomic<unsigned> next{0};
            name = "cpp_graph_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())
                    + "_" + std::to_string(next++);
            cursor.set_fetch_size(1);
            auto created = cursor.execute("sql", "CREATE GRAPH NAMESPACE " + name, "public");
            REQUIRE(created->get_result_type() == Results::ResultType::SCALAR);
            query("CREATE (:Person {id: 1, name: 'Alice'})-[:KNOWS {since: 2020}]->"
                  "(:Person {id: 2, name: 'Bob'})-[:KNOWS {since: 2021}]->"
                  "(:Person {id: 3, name: 'Charlie'}), (:Person {id: 4, name: 'Isolated'})");
        }

        ~GraphFixture() {
            try {
                Connection::Cursor cleanup(*connection);
                cleanup.execute("sql", "DROP NAMESPACE IF EXISTS " + name, "public");
            } catch (const std::exception &error) {
                std::cerr << "Graph test cleanup failed for " << name << ": " << error.what() << '\n';
            }
        }

        std::unique_ptr<Results::Result> query(const std::string &cypher) {
            return cursor.execute("cypher", cypher, name);
        }

        std::vector<std::string> names() {
            auto result = query("MATCH (n:Person) RETURN n.name AS name ORDER BY name");
            std::vector<std::string> values;
            for (auto &row : *result->unwrap<Results::RelationalResult>()) {
                values.push_back(row["name"].as_string());
            }
            return values;
        }
    };

    class RelationalFixture {
    public:
        std::unique_ptr<Connection::Connection> connection = open_connection();
        Connection::Cursor cursor{*connection};
        std::string name = "cpp_scalar_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());

        RelationalFixture() {
            cursor.execute("sql", "CREATE TABLE " + name + " (v INTEGER PRIMARY KEY)", "public");
            cursor.execute("sql", "INSERT INTO " + name + " VALUES (1), (2), (3)", "public");
        }

        ~RelationalFixture() {
            try {
                Connection::Cursor cleanup(*connection);
                cleanup.execute("sql", "DROP TABLE IF EXISTS " + name, "public");
            } catch (const std::exception &error) {
                std::cerr << "Scalar test cleanup failed: " << error.what() << '\n';
            }
        }
    };

    int64_t integer(Types::TypedValue value) {
        if (value.get_value_case() == org::polypheny::prism::ProtoValue::kString) {
            return std::stoll(value.as_string());
        }
        if (value.get_value_case() == org::polypheny::prism::ProtoValue::kInteger) {
            return value.as_int32_t();
        }
        return value.as_int64_t();
    }

}

TEST_CASE("Live Cypher iterates graph node pages and keeps graph metadata", "[.graph-integration]") {
    GraphFixture fixture;
    const auto version = fixture.connection->get_server_api_version();
    REQUIRE(version.first == 2);
    REQUIRE(version.second >= 3);
    auto entities = fixture.connection->search_entities(fixture.name);
    REQUIRE(entities.entities_size() == 1);
    REQUIRE(entities.entities(0).has_graph());
    auto result = fixture.query("MATCH (n:Person) RETURN n ORDER BY n.id");
    REQUIRE(result->get_result_type() == Results::ResultType::GRAPH);
    std::vector<std::string> names;
    for (auto element : *result->unwrap<Results::GraphResult>()) {
        REQUIRE(element->get_element_type() == GraphElementType::NODE);
        auto node = element->unwrap<Results::Node>();
        REQUIRE_FALSE(node.get_id().empty());
        CHECK(node.get_labels() == std::vector<std::string>{"Person"});
        names.push_back(node.at("name").as_string());
    }
    CHECK(names == std::vector<std::string>{"Alice", "Bob", "Charlie", "Isolated"});
}

TEST_CASE("Live Cypher edges preserve source target labels direction and properties", "[.graph-integration]") {
    GraphFixture fixture;
    auto result = fixture.query(
            "MATCH (a:Person {name: 'Alice'})-[r:KNOWS]->(b:Person) RETURN a, r, b");
    std::vector<std::shared_ptr<Results::GraphElement>> elements;
    for (auto element : *result->unwrap<Results::GraphResult>()) {
        elements.push_back(std::move(element));
    }
    REQUIRE(elements.size() == 3);
    auto source = elements[0]->unwrap<Results::Node>();
    auto edge = elements[1]->unwrap<Results::Edge>();
    auto target = elements[2]->unwrap<Results::Node>();
    CHECK(source.at("name").as_string() == "Alice");
    CHECK(target.at("name").as_string() == "Bob");
    CHECK(edge.get_left() == source.get_id());
    CHECK(edge.get_right() == target.get_id());
    CHECK(edge.get_direction() == Results::EdgeDirection::LEFT_TO_RIGHT);
    CHECK(edge.get_labels() == std::vector<std::string>{"KNOWS"});
    CHECK(integer(edge.at("since")) == 2020);
}

TEST_CASE("Live Cypher standalone paths stay in traversal order", "[.graph-integration]") {
    GraphFixture fixture;
    auto result = fixture.query(
            "MATCH p=(:Person {name: 'Alice'})-[:KNOWS*1..3]->(:Person {name: 'Charlie'}) RETURN p");
    std::vector<GraphElementType> types;
    std::vector<std::string> names;
    for (auto element : *result->unwrap<Results::GraphResult>()) {
        types.push_back(element->get_element_type());
        if (element->get_element_type() == GraphElementType::NODE) {
            names.push_back(element->at("name").as_string());
        }
    }
    CHECK(types == std::vector<GraphElementType>{GraphElementType::NODE, GraphElementType::EDGE,
          GraphElementType::NODE, GraphElementType::EDGE, GraphElementType::NODE});
    CHECK(names == std::vector<std::string>{"Alice", "Bob", "Charlie"});
}

TEST_CASE("Live Cypher full graph scans include isolated nodes", "[.graph-integration]") {
    GraphFixture fixture;
    auto result = fixture.query("MATCH (*) RETURN *");
    size_t nodes = 0;
    size_t edges = 0;
    bool isolated = false;
    for (auto element : *result->unwrap<Results::GraphResult>()) {
        if (element->get_element_type() == GraphElementType::NODE) {
            ++nodes;
            isolated |= element->at("name").as_string() == "Isolated";
        } else {
            ++edges;
        }
    }
    CHECK(nodes == 4);
    CHECK(edges == 2);
    CHECK(isolated);
}

TEST_CASE("Live Cypher scalar projections and aggregates preserve relational columns", "[.graph-integration]") {
    GraphFixture fixture;
    CHECK(fixture.names() == std::vector<std::string>{"Alice", "Bob", "Charlie", "Isolated"});
    auto result = fixture.query("MATCH (n:Person) RETURN count(n) AS count");
    auto *rows = result->unwrap<Results::RelationalResult>();
    CHECK(rows->get_metadata()->get_column_meta(0).get_column_label() == "count");
    REQUIRE(rows->begin() != rows->end());
    CHECK(integer((*rows->begin())["count"]) == 4);
    result->close();
    auto literals = fixture.query("RETURN [1, 2, null] AS values");
    auto *literal_rows = literals->unwrap<Results::RelationalResult>();
    CHECK(literal_rows->get_metadata()->get_column_meta(0).get_protocol_type_name() == "ARRAY");
    auto values = (*literal_rows->begin())["values"].as_list();
    REQUIRE(values.size() == 3);
    auto value = values.begin();
    CHECK(value++->as_big_decimal() == 1);
    CHECK(value++->as_big_decimal() == 2);
    CHECK(value->is_null());
}

TEST_CASE("Live Cypher mixed node and scalar columns work in both projection orders", "[.graph-integration]") {
    GraphFixture fixture;
    for (bool node_first : {false, true}) {
        auto result = fixture.query(std::string("MATCH (n:Person) RETURN ")
                + (node_first ? "n, n.name AS name" : "n.name AS name, n") + " ORDER BY name");
        auto *rows = result->unwrap<Results::RelationalResult>();
        const auto node_index = node_first ? 0 : 1;
        REQUIRE(rows->get_metadata()->get_column_count() == 2);
        CHECK(rows->get_metadata()->get_column_meta(node_index).get_protocol_type_name() == "NODE");
        std::vector<std::string> names;
        for (auto &row : *rows) {
            auto node = row["n"].as_node();
            CHECK(node.at("name").as_string() == row["name"].as_string());
            names.push_back(row["name"].as_string());
        }
        CHECK(names == std::vector<std::string>{"Alice", "Bob", "Charlie", "Isolated"});
    }
}

TEST_CASE("Live Cypher mixed endpoint edge and scalar values remain typed", "[.graph-integration]") {
    GraphFixture fixture;
    auto result = fixture.query("MATCH (a:Person {name: 'Alice'})-[r:KNOWS]->(b:Person) RETURN a, r, b, b.name AS name");
    auto *rows = result->unwrap<Results::RelationalResult>();
    REQUIRE(rows->begin() != rows->end());
    auto &row = *rows->begin();
    auto source = row["a"].as_node();
    auto edge = row["r"].as_edge();
    auto target = row["b"].as_node();
    CHECK(edge.get_left() == source.get_id());
    CHECK(edge.get_right() == target.get_id());
    CHECK(integer(edge.at("since")) == 2020);
    CHECK(row["name"].as_string() == target.at("name").as_string());
    CHECK(rows->get_metadata()->get_column_meta(1).get_protocol_type_name() == "EDGE");
}

TEST_CASE("Live Cypher paths remain one ordered cell in mixed projections", "[.graph-integration]") {
    GraphFixture fixture;
    auto result = fixture.query(
            "MATCH p=(:Person {name: 'Alice'})-[:KNOWS*1..3]->(:Person {name: 'Charlie'}) RETURN p, 'route' AS title");
    auto *rows = result->unwrap<Results::RelationalResult>();
    REQUIRE(rows->begin() != rows->end());
    auto path = (*rows->begin())["p"].as_path();
    CHECK((*rows->begin())["title"].as_string() == "route");
    CHECK(rows->get_metadata()->get_column_meta(0).get_protocol_type_name() == "PATH");
    REQUIRE(path.get_nodes().size() == 3);
    REQUIRE(path.get_edges().size() == 2);
    auto first_node = path.get_nodes()[0];
    CHECK(first_node.at("name").as_string() == "Alice");
    CHECK(path.get_edges()[0].get_left() == path.get_nodes()[0].get_id());
    CHECK(path.get_edges()[1].get_right() == path.get_nodes()[2].get_id());

    result->close();
    auto reversed = fixture.query(
            "MATCH p=(:Person {name: 'Charlie'})<-[:KNOWS*1..3]-(:Person {name: 'Alice'}) RETURN p, 'reverse' AS title");
    auto *reverse_rows = reversed->unwrap<Results::RelationalResult>();
    auto reverse_path = (*reverse_rows->begin())["p"].as_path();
    REQUIRE(reverse_path.get_nodes().size() == 3);
    REQUIRE(reverse_path.get_edges().size() == 2);
    auto reverse_start = reverse_path.get_nodes()[0];
    CHECK(reverse_start.at("name").as_string() == "Charlie");
    CHECK(reverse_path.get_edges()[0].get_right() == reverse_path.get_nodes()[0].get_id());
    CHECK(reverse_path.get_edges()[0].get_left() == reverse_path.get_nodes()[1].get_id());
    CHECK(reverse_path.get_edges()[1].get_left() == reverse_path.get_nodes()[2].get_id());
    CHECK(reverse_path.get_edges()[0].get_direction() == Results::EdgeDirection::LEFT_TO_RIGHT);
}

TEST_CASE("Live Cypher no-match results retain their distinct result shapes", "[.graph-integration]") {
    GraphFixture fixture;
    {
        auto result = fixture.query("MATCH (n:Person {name: 'Nobody'}) RETURN n");
        auto *graph = result->unwrap<Results::GraphResult>();
        CHECK(graph->begin() == graph->end());
    }
    {
        auto result = fixture.query("MATCH (n:Person {name: 'Nobody'}) RETURN n, n.name AS name");
        auto *rows = result->unwrap<Results::RelationalResult>();
        CHECK(rows->begin() == rows->end());
        CHECK(rows->get_metadata()->get_column_count() == 2);
        CHECK(rows->get_metadata()->get_column_meta(0).get_protocol_type_name() == "NODE");
    }
}

TEST_CASE("Live Cypher early result disposal allows subsequent queries", "[.graph-integration]") {
    GraphFixture fixture;
    {
        auto result = fixture.query("MATCH (n:Person) RETURN n ORDER BY n.id");
        auto *graph = result->unwrap<Results::GraphResult>();
        CHECK(graph->begin() != graph->end());
        result->close();
        result->close();
    }
    CHECK(fixture.names().size() == 4);
    {
        auto result = fixture.query("MATCH (n:Person) RETURN n, n.name AS name ORDER BY name");
        result->close();
    }
    CHECK(fixture.names().size() == 4);
}

TEST_CASE("Live Cypher errors and rejected preparation leave the connection usable", "[.graph-integration]") {
    GraphFixture fixture;
    CHECK_THROWS_AS(fixture.query("THIS IS NOT CYPHER"), std::exception);
    CHECK(fixture.names().size() == 4);
    CHECK_THROWS_AS(fixture.query("MATCH (n) RETURN n; MATCH (n) RETURN n"), std::exception);
    CHECK(fixture.names().size() == 4);
    CHECK_THROWS_AS(fixture.cursor.prepare_indexed("cypher", "MATCH (n:Person) RETURN n", fixture.name), std::exception);
    CHECK(fixture.names().size() == 4);
    CHECK_THROWS_AS(fixture.cursor.prepare_named("cypher", "MATCH (n:Person) RETURN n", fixture.name), std::exception);
    CHECK(fixture.names().size() == 4);
}

TEST_CASE("Live Cypher batch writes and empty batches complete", "[.graph-integration]") {
    GraphFixture fixture;
    const auto counts = fixture.cursor.execute("cypher", std::vector<std::string>{
        "CREATE (:Extra {name: 'One'})", "CREATE (:Extra {name: 'Two'})"}, fixture.name);
    CHECK(counts.size() == 2);
    CHECK(fixture.cursor.execute("cypher", std::vector<std::string>{}, fixture.name).empty());
    auto result = fixture.query("MATCH (n:Extra) RETURN count(n) AS count");
    CHECK(integer((*result->unwrap<Results::RelationalResult>()->begin())["count"]) == 2);
}

TEST_CASE("Live Cypher graph result cleanup preserves manual transaction control", "[.graph-integration]") {
    GraphFixture fixture;
    auto connection = open_connection(false);
    Connection::Cursor cursor(*connection);
    cursor.set_fetch_size(1);
    cursor.execute("cypher", "CREATE (:Uncommitted {name: 'Transient'})", fixture.name);
    {
        auto result = cursor.execute("cypher", "MATCH (n:Person) RETURN n", fixture.name);
        result->close();
    }
    connection->rollback();
    {
        auto result = cursor.execute("cypher", "MATCH (n:Uncommitted) RETURN count(n) AS count", fixture.name);
        CHECK(integer((*result->unwrap<Results::RelationalResult>()->begin())["count"]) == 0);
    }
    connection->commit();
}
TEST_CASE("Live Cypher decimals preserve negative and large scalar values", "[.graph-integration]") {
    GraphFixture fixture;
    auto result = fixture.query("RETURN -1 AS negative, 128 AS positive, -1.25 AS fraction, 0 AS zero");
    auto &row = *result->unwrap<Results::RelationalResult>()->begin();
    CHECK(row["negative"].as_decimal_string() == "-1");
    CHECK(row["negative"].as_big_decimal() == -1);
    CHECK(row["positive"].as_decimal_string() == "128");
    CHECK(row["fraction"].as_double() == -1.25);
    CHECK(row["zero"].as_decimal_string() == "0");
    result->close();
    RelationalFixture sql;
    auto large = sql.cursor.execute("sql", "SELECT CAST('-123456789012345678901234567890.12345678' AS DECIMAL(38,8)) FROM " + sql.name + " WHERE v = 1", "public");
    CHECK((*large->unwrap<Results::RelationalResult>()->begin())[0].as_decimal_string()
          == "-123456789012345678901234567890.12345678");
}

TEST_CASE("Live results retain statement ownership when their cursor is destroyed", "[.graph-integration]") {
    GraphFixture fixture;
    std::unique_ptr<Results::Result> result;
    {
        Connection::Cursor temporary(*fixture.connection);
        temporary.set_fetch_size(1);
        result = temporary.execute("cypher", "MATCH (n:Person) RETURN n", fixture.name);
    }
    size_t count = 0;
    for (auto node : *result->unwrap<Results::GraphResult>()) {
        CHECK(node->get_element_type() == GraphElementType::NODE);
        ++count;
    }
    CHECK(count == 4);
    result.reset();
    CHECK(fixture.names().size() == 4);
}

TEST_CASE("Live prepared SQL replacement does not let an old result close the new one", "[.graph-integration]") {
    RelationalFixture fixture;
    Connection::Cursor cursor(*fixture.connection);
    cursor.set_fetch_size(1);
    cursor.prepare_indexed("sql", "SELECT v FROM " + fixture.name + " WHERE v >= ? ORDER BY v", "public");
    std::vector<Types::TypedValue> parameters{Types::TypedValue(2)};
    auto first = cursor.execute_prepared(parameters);
    auto second = cursor.execute_prepared(parameters);
    CHECK_THROWS_AS(first->unwrap<Results::RelationalResult>()->begin(), std::runtime_error);
    first.reset();
    size_t count = 0;
    for (auto &row : *second->unwrap<Results::RelationalResult>()) {
        CHECK(row.size() == 1);
        ++count;
    }
    CHECK(count == 2);
}
#endif
