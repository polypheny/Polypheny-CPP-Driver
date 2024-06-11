#include <catch2/catch_all.hpp>
#include "src/utils/ProtoUtils.h"
#include "value.pb.h"
#include "src/types/TypedValue.h"

constexpr long long MILLIS_PER_DAY = 86400000LL;

TEST_CASE("ProtoUtils::string_to_vector", "[ProtoUtils]") {
    std::string original = "test string";
    std::vector<uint8_t> vec = Utils::ProtoUtils::string_to_vector(original);

    REQUIRE(std::string(vec.begin(), vec.end()) == original);
}

TEST_CASE("ProtoUtils::vector_to_string", "[ProtoUtils]") {
    std::vector<uint8_t> vec = {'t', 'e', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g'};
    std::string converted_back = Utils::ProtoUtils::vector_to_string(vec);

    REQUIRE(converted_back == "test string");
}

TEST_CASE("ProtoUtils::proto_to_timestamp", "[ProtoUtils]") {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    auto proto_timestamp = Utils::ProtoUtils::timestamp_to_proto(now);
    auto converted_back = Utils::ProtoUtils::proto_to_timestamp(*proto_timestamp);

    REQUIRE(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() ==
            std::chrono::duration_cast<std::chrono::milliseconds>(converted_back.time_since_epoch()).count());
}

TEST_CASE("ProtoUtils::timestamp_to_proto", "[ProtoUtils]") {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    auto proto_timestamp = Utils::ProtoUtils::timestamp_to_proto(now);

    REQUIRE(proto_timestamp->timestamp() ==
            std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
}

TEST_CASE("ProtoUtils::proto_to_date", "[ProtoUtils]") {
    std::chrono::system_clock::time_point today = std::chrono::system_clock::now();

    auto proto_date = Utils::ProtoUtils::date_to_proto(today);
    auto converted_back = Utils::ProtoUtils::proto_to_date(*proto_date);

    uint64_t expected =
            (std::chrono::duration_cast<std::chrono::milliseconds>(today.time_since_epoch()).count() / MILLIS_PER_DAY) *
            MILLIS_PER_DAY;
    REQUIRE(expected ==
            std::chrono::duration_cast<std::chrono::milliseconds>(converted_back.time_since_epoch()).count());
}

TEST_CASE("ProtoUtils::date_to_proto", "[ProtoUtils]") {
    std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
    auto proto_date = Utils::ProtoUtils::date_to_proto(today);

    uint64_t millis_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
            today.time_since_epoch()).count();
    REQUIRE(proto_date->date() == millis_since_epoch / MILLIS_PER_DAY);
}

TEST_CASE("ProtoUtils::proto_to_time", "[ProtoUtils]") {
    std::chrono::milliseconds time_duration(123456);
    auto proto_time = Utils::ProtoUtils::time_to_proto(time_duration);
    auto converted_back = Utils::ProtoUtils::proto_to_time(*proto_time);

    REQUIRE(time_duration.count() == converted_back.count());
}

TEST_CASE("ProtoUtils::time_to_proto", "[ProtoUtils]") {
    std::chrono::milliseconds time_duration(123456);
    auto proto_time = Utils::ProtoUtils::time_to_proto(time_duration);

    REQUIRE(proto_time->time() == time_duration.count());
}

TEST_CASE("ProtoUtils::proto_to_list", "[ProtoUtils]") {
    SKIP("Broken due to TypedValue not working correctly!");
    org::polypheny::prism::ProtoList proto_list;
    auto *value1 = proto_list.add_values();
    value1->mutable_integer()->set_integer(42);
    auto *value2 = proto_list.add_values();
    value2->mutable_string()->set_string("value");

    std::list<Types::TypedValue> converted_back = Utils::ProtoUtils::proto_to_list(proto_list);

    REQUIRE(1 < 2);
    REQUIRE(converted_back.size() == 2);
    auto it = converted_back.begin();
    REQUIRE(it->as_int32_t() == 42);
    ++it;
    REQUIRE(it->as_string() == "value");
}

TEST_CASE("ProtoUtils::list_to_proto", "[ProtoUtils]") {
    SKIP("Broken due to TypedValue not working correctly!");
    std::list<Types::TypedValue> original_list = {Types::TypedValue(42), Types::TypedValue(std::string("value"))};
    auto proto_list = Utils::ProtoUtils::list_to_proto(original_list);

    REQUIRE(proto_list->values_size() == 2);
    REQUIRE(proto_list->values(0).integer().integer() == 42);
    REQUIRE(proto_list->values(1).string().string() == "value");
}

TEST_CASE("Utils::bytes_to_mpf", "[Utils]") {
    std::string input = "\x01"; // Represents the number 1 in bytes
    mpf_class expected("1");

    mpf_class result = Utils::ProtoUtils::bytes_to_mpf(input);

    REQUIRE(result == expected);
}

TEST_CASE("Utils::scale_by_10_to_neg_x", "[Utils]") {
    mpf_class number("12345");
    int32_t x = 2;
    mpf_class expected("123.45");

    Utils::ProtoUtils::scale_by_10_to_neg_x(number, x);

    REQUIRE(number == expected);
}

TEST_CASE("Utils::mpf_get_unscaled_value_and_scale", "[Utils]") {
    mpf_class number("123.45");
    std::pair<std::string, int32_t> result = Utils::ProtoUtils::mpf_get_unscaled_value_and_scale(number);

    // decimal 12345 -> hex 0x30 0x39 -> ascii '0' = 0x30, '9' = 0x39
    std::string expected_unscaled_value = R"(09)";
    uint32_t expected_scale = 2;

    REQUIRE(result.first == expected_unscaled_value);
    REQUIRE(result.second == expected_scale);
}

