#include <catch2/catch_all.hpp>
#include <PolyphenyCPPDriver.h>
#include <sstream>

#if !POLYPHENY_PRISM_STREAMING
namespace {

    org::polypheny::prism::ProtoNode node(std::string id, std::string label = "Person") {
        org::polypheny::prism::ProtoNode value;
        value.set_id(std::move(id));
        value.set_name("n");
        value.add_labels(std::move(label));
        (*value.mutable_properties())["name"].mutable_string()->set_string("Alice");
        (*value.mutable_properties())["age"].mutable_integer()->set_integer(42);
        (*value.mutable_properties())["missing"].mutable_null();
        return value;
    }

    org::polypheny::prism::ProtoEdge edge() {
        org::polypheny::prism::ProtoEdge value;
        value.set_id("edge-1");
        value.set_name("r");
        value.add_labels("KNOWS");
        value.set_left("alice");
        value.set_right("bob");
        value.set_direction(org::polypheny::prism::ProtoEdge::LEFT_TO_RIGHT);
        (*value.mutable_properties())["since"].mutable_long_()->set_long_(2020);
        return value;
    }

    org::polypheny::prism::ProtoPath path(bool reverse = false) {
        org::polypheny::prism::ProtoPath value;
        *value.add_nodes() = node(reverse ? "bob" : "alice");
        *value.add_nodes() = node(reverse ? "alice" : "bob");
        *value.add_edges() = edge();
        return value;
    }

    void preserves_value(org::polypheny::prism::ProtoValue encoded) {
        Types::TypedValue lazy(encoded, nullptr);
        Types::TypedValue copied(lazy);
        Types::TypedValue assigned;
        assigned = lazy;
        for (auto *value : {&lazy, &copied, &assigned}) {
            std::unique_ptr<org::polypheny::prism::ProtoValue> serialized(value->serialize(nullptr));
            CHECK(serialized->DebugString() == encoded.DebugString());
            Types::TypedValue materialized(*value);
            std::unique_ptr<org::polypheny::prism::ProtoValue> copied_serialized(materialized.serialize(nullptr));
            CHECK(copied_serialized->DebugString() == encoded.DebugString());
        }
    }

}

TEST_CASE("Typed graph nodes preserve identity labels properties and copies", "[graph]") {
    org::polypheny::prism::ProtoValue encoded;
    *encoded.mutable_node() = node("alice");
    Types::TypedValue value(encoded, nullptr);
    auto decoded = value.as_node();
    CHECK(decoded.get_id() == "alice");
    CHECK(decoded.get_name() == "n");
    CHECK(decoded.get_labels() == std::vector<std::string>{"Person"});
    CHECK(decoded.at("name").as_string() == "Alice");
    CHECK(decoded.at("age").as_int32_t() == 42);
    CHECK(decoded.at("missing").is_null());
    CHECK(decoded.get_element_type() == GraphElementType::NODE);
    CHECK(decoded.unwrap<Results::Node>().get_id() == "alice");
    CHECK_THROWS_AS(decoded.unwrap<Results::Edge>(), std::runtime_error);
    CHECK_THROWS_AS(value.as_edge(), std::runtime_error);
    CHECK_THROWS_AS(value.as_path(), std::runtime_error);
    CHECK_THROWS_AS(value.as_graph(), std::runtime_error);
    decoded["age"] = Types::TypedValue(43);
    CHECK(value.as_node().at("age").as_int32_t() == 42);
    preserves_value(encoded);
    Types::TypedValue constructed(value.as_node());
    CHECK(constructed.as_node().at("name").as_string() == "Alice");
    std::ostringstream output;
    output << value;
    CHECK(output.str() == "Node(alice)");
}

TEST_CASE("Typed graph edges preserve direction endpoints and labels", "[graph]") {
    for (auto direction : {org::polypheny::prism::ProtoEdge::LEFT_TO_RIGHT,
                           org::polypheny::prism::ProtoEdge::RIGHT_TO_LEFT,
                           org::polypheny::prism::ProtoEdge::NONE}) {
        org::polypheny::prism::ProtoValue encoded;
        *encoded.mutable_edge() = edge();
        encoded.mutable_edge()->set_direction(direction);
        Types::TypedValue value(encoded, nullptr);
        auto decoded = value.as_edge();
        CHECK(decoded.get_id() == "edge-1");
        CHECK(decoded.get_name() == "r");
        CHECK(decoded.get_left() == "alice");
        CHECK(decoded.get_right() == "bob");
        CHECK(decoded.get_labels() == std::vector<std::string>{"KNOWS"});
        CHECK(decoded.at("since").as_int64_t() == 2020);
        CHECK(decoded.get_element_type() == GraphElementType::EDGE);
        preserves_value(encoded);
        Types::TypedValue constructed(decoded);
        CHECK(constructed.as_edge().get_direction() == decoded.get_direction());
        CHECK_THROWS_AS(value.as_node(), std::runtime_error);
    }
    auto invalid = edge();
    invalid.set_direction(org::polypheny::prism::ProtoEdge::UNSPECIFIED);
    CHECK_THROWS_AS(Results::Edge(invalid, nullptr), std::runtime_error);
}

TEST_CASE("Graph paths preserve ordered traversal and original edge orientation", "[graph]") {
    for (bool reverse : {false, true}) {
        org::polypheny::prism::ProtoValue encoded;
        *encoded.mutable_path() = path(reverse);
        Types::TypedValue value(encoded, nullptr);
        const auto decoded = value.as_path();
        REQUIRE(decoded.get_nodes().size() == 2);
        REQUIRE(decoded.get_edges().size() == 1);
        CHECK(decoded.get_nodes()[0].get_id() == (reverse ? "bob" : "alice"));
        CHECK(decoded.get_edges()[0].get_left() == "alice");
        CHECK(decoded.get_edges()[0].get_right() == "bob");
        preserves_value(encoded);
        Types::TypedValue constructed(decoded);
        CHECK(constructed.as_path().get_nodes()[1].get_id() == (reverse ? "alice" : "bob"));
    }
    org::polypheny::prism::ProtoPath malformed;
    CHECK_THROWS_AS(Results::Path(malformed), std::runtime_error);
    *malformed.add_nodes() = node("one");
    CHECK(Results::Path(malformed).get_edges().empty());
}

TEST_CASE("Graph values keep isolated nodes and empty graphs", "[graph]") {
    org::polypheny::prism::ProtoValue encoded;
    auto *graph = encoded.mutable_graph();
    *graph->add_nodes() = node("alice");
    *graph->add_nodes() = node("bob");
    *graph->add_nodes() = node("isolated");
    *graph->add_edges() = edge();
    Types::TypedValue value(encoded, nullptr);
    REQUIRE(value.as_graph().get_nodes().size() == 3);
    REQUIRE(value.as_graph().get_edges().size() == 1);
    CHECK(value.as_graph().get_nodes()[2].get_id() == "isolated");
    preserves_value(encoded);
    Types::TypedValue constructed(value.as_graph());
    CHECK(constructed.as_graph().get_edges()[0].get_left() == "alice");
    encoded.mutable_graph()->Clear();
    Types::TypedValue empty(encoded, nullptr);
    CHECK(empty.as_graph().get_nodes().empty());
    CHECK(empty.as_graph().get_edges().empty());
    preserves_value(encoded);
}

TEST_CASE("Graph values remain typed inside nested lists and documents", "[graph]") {
    org::polypheny::prism::ProtoValue encoded;
    auto *list = (*encoded.mutable_document()->mutable_entries())["values"].mutable_list();
    *list->add_values()->mutable_node() = node("alice");
    *list->add_values()->mutable_edge() = edge();
    *list->add_values()->mutable_path() = path();
    *list->add_values()->mutable_graph()->add_nodes() = node("isolated");
    list->add_values()->mutable_null();
    Types::TypedValue value(encoded, nullptr);
    auto values = value.as_document().at("values").as_list();
    auto item = values.begin();
    CHECK(item++->as_node().get_id() == "alice");
    CHECK(item++->as_edge().get_id() == "edge-1");
    CHECK(item++->as_path().get_nodes().size() == 2);
    CHECK(item++->as_graph().get_nodes()[0].get_id() == "isolated");
    CHECK(item->is_null());
    preserves_value(encoded);
    org::polypheny::prism::ProtoValue unknown;
    Types::TypedValue missing(unknown, nullptr);
    CHECK_THROWS_AS(missing.serialize(nullptr), std::runtime_error);
}

TEST_CASE("Graph property decimals retain their exact signed wire representation", "[graph][decimal-regression]") {
    org::polypheny::prism::ProtoValue encoded;
    *encoded.mutable_node() = node("alice");
    auto *amount = (*encoded.mutable_node()->mutable_properties())["amount"].mutable_big_decimal();
    amount->set_unscaled_value(std::string{char(0xff), char(0x7f)});
    amount->set_scale(3);
    Types::TypedValue value(encoded, nullptr);
    CHECK(value.as_node().at("amount").as_decimal_string() == "-0.129");
    preserves_value(encoded);
}
#endif
