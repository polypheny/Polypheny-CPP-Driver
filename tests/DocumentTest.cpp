#include <catch2/catch_all.hpp>
#include "src/types/Document.h"
#include "src/types/TypedValue.h"
#include "value.pb.h"

TEST_CASE("Document Initialization from ProtoDocument 1", "[document]") {
    org::polypheny::prism::ProtoDocument proto_doc;
    auto entry = proto_doc.add_entries();
    entry->mutable_key()->mutable_string()->set_string("key1");
    entry->mutable_value()->mutable_integer()->set_integer(42);

    Types::Document doc(proto_doc);

    REQUIRE(doc.size() == 1);
    REQUIRE(doc["key1"].as_int32_t() == 42);
    REQUIRE(doc.at("key1").as_int32_t() == 42);
}

TEST_CASE("Document Serialization", "[document]") {
    Types::Document doc;
    doc["key1"] = Types::TypedValue(42);
    doc["key2"] = Types::TypedValue(std::string("value"));

    auto proto_doc = doc.serialize();

    REQUIRE(proto_doc->entries_size() == 2);

    // order insensitive checking of keys and their values
    std::set<std::string> keys;
    std::set<int> int_values;
    std::set<std::string> string_values;

    for (const auto& entry : proto_doc->entries()) {
        keys.insert(entry.key().string().string());
        if (entry.value().has_integer()) {
            int_values.insert(entry.value().integer().integer());
        } else if (entry.value().has_string()) {
            string_values.insert(entry.value().string().string());
        }
    }

    REQUIRE(keys.size() == 2);
    REQUIRE(keys.count("key1") == 1);
    REQUIRE(keys.count("key2") == 1);

    REQUIRE(int_values.size() == 1);
    REQUIRE(int_values.count(42) == 1);

    REQUIRE(string_values.size() == 1);
    REQUIRE(string_values.count("value") == 1);
}

TEST_CASE("Document Insert and Erase", "[document]") {
    Types::Document doc;

    doc.insert({"key1", Types::TypedValue(42)});
    doc.insert({"key2", Types::TypedValue(std::string("value"))});

    REQUIRE(doc.size() == 2);
    REQUIRE(doc["key1"].as_int32_t() == 42);
    REQUIRE(doc["key2"].as_string() == "value");

    doc.erase("key1");

    REQUIRE(doc.size() == 1);
    REQUIRE(doc.find("key1") == doc.end());
    REQUIRE(doc["key2"].as_string() == "value");
}

TEST_CASE("Document Find", "[document]") {
    Types::Document doc;
    doc["key1"] = Types::TypedValue(42);

    auto it = doc.find("key1");

    REQUIRE(it != doc.end());
    REQUIRE(it->second.as_int32_t() == 42);

    Types::Document& const_doc = doc;
    auto const_it = const_doc.find("key1");

    REQUIRE(const_it != const_doc.end());
    REQUIRE(const_it->second.as_int32_t() == 42);
}

TEST_CASE("Document Size and Empty", "[document]") {
    Types::Document doc;

    REQUIRE(doc.empty());
    REQUIRE(doc.size() == 0);

    doc["key1"] = Types::TypedValue(42);

    REQUIRE(!doc.empty());
    REQUIRE(doc.size() == 1);
}

TEST_CASE("Document Clear", "[document]") {
    Types::Document doc;
    doc["key1"] = Types::TypedValue(42);
    doc["key2"] = Types::TypedValue(std::string("value"));

    REQUIRE(doc.size() == 2);

    doc.clear();

    REQUIRE(doc.empty());
    REQUIRE(doc.size() == 0);
}

TEST_CASE("Document Iterators", "[document]") {
    Types::Document doc;
    doc["key1"] = Types::TypedValue(42);
    doc["key2"] = Types::TypedValue(std::string("value"));

    auto it = doc.begin();
    REQUIRE(it != doc.end());

    ++it;
    REQUIRE(it != doc.end());

    ++it;
    REQUIRE(it == doc.end());
}
