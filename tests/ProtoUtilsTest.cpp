#include <catch2/catch_all.hpp>
#include <limits>
#include "src/utils/ProtoUtils.h"
#include "org/polypheny/prism/value.pb.h"
#include "src/types/TypedValue.h"
#include "src/types/Interval.h"

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
    org::polypheny::prism::ProtoList proto_list;
    auto *value1 = proto_list.add_values();
    value1->mutable_integer()->set_integer(42);
    auto *value2 = proto_list.add_values();
    value2->mutable_string()->set_string("value");

    std::shared_ptr<Communication::PrismInterfaceClient> client;
    std::list<Types::TypedValue> converted_back = Utils::ProtoUtils::proto_to_list(proto_list, client);

    REQUIRE(converted_back.size() == 2);
    auto it = converted_back.begin();
    REQUIRE(it->as_int32_t() == 42);
    ++it;
    REQUIRE(it->as_string() == "value");
}

TEST_CASE("ProtoUtils::list_to_proto", "[ProtoUtils]") {
    std::list<Types::TypedValue> original_list = {Types::TypedValue(42), Types::TypedValue(std::string("value"))};

    std::shared_ptr<Streaming::StreamingIndex> index;
    auto proto_list = Utils::ProtoUtils::list_to_proto(original_list, index);

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

TEST_CASE("Utils::mpf_get_unscaled_value_and_scale - positive scale", "[Utils]") {
    mpf_class number("123.45");
    std::pair<std::string, int32_t> result = Utils::ProtoUtils::mpf_get_unscaled_value_and_scale(number);

    std::string expected_unscaled_value = std::string{char(0x30), char(0x39)};
    int32_t expected_scale = 2;

    REQUIRE(result.first == expected_unscaled_value);
    REQUIRE(result.second == expected_scale);
}

TEST_CASE("Utils::mpf_get_unscaled_value_and_scale - negative scale", "[Utils]") {
    mpf_class number("1234500");
    std::pair<std::string, int32_t> result = Utils::ProtoUtils::mpf_get_unscaled_value_and_scale(number);

    std::string expected_unscaled_value = std::string{char(0x30), char(0x39)};;
    int32_t expected_scale = -2;

    REQUIRE(result.first == expected_unscaled_value);
    REQUIRE(result.second == expected_scale);
}

TEST_CASE("Utils::mpf_get_unscaled_value_and_scale - scale 0", "[Utils]") {
    mpf_class number("123");
    std::pair<std::string, int32_t> result = Utils::ProtoUtils::mpf_get_unscaled_value_and_scale(number);

    std::string expected_unscaled_value = std::string{char(0x7B)};
    int32_t expected_scale = 0;

    REQUIRE(result.first == expected_unscaled_value);
    REQUIRE(result.second == expected_scale);
}


TEST_CASE("Prism decimal integers use signed two's-complement bytes", "[decimal-regression]") {
    CHECK(Utils::ProtoUtils::bytes_to_mpf(std::string(1, char(0xff))) == -1);
    CHECK(Utils::ProtoUtils::bytes_to_mpf(std::string{char(0x00), char(0x80)}) == 128);
    CHECK(Utils::ProtoUtils::bytes_to_mpf(std::string{char(0xff), char(0x7f)}) == -129);
    for (const auto &entry : std::vector<std::pair<std::string, std::string>>{
            {"-1", std::string(1, char(0xff))}, {"128", std::string{char(0x00), char(0x80)}},
            {"-129", std::string{char(0xff), char(0x7f)}}, {"0", std::string(1, '\0')}}) {
        const auto encoded = Utils::ProtoUtils::mpf_get_unscaled_value_and_scale(mpf_class(entry.first));
        CHECK(encoded.first == entry.second);
        CHECK(encoded.second == 0);
    }
    mpf_class number(123);
    Utils::ProtoUtils::scale_by_10_to_neg_x(number, -3);
    CHECK(number == 123000);
}

TEST_CASE("Dates preserve pre-epoch days and reject system clock overflow", "[ProtoUtils]") {
    using Clock = std::chrono::system_clock;
    const auto before_epoch = Clock::time_point(std::chrono::milliseconds(-1));
    CHECK(Utils::ProtoUtils::date_to_proto(before_epoch)->date() == -1);
    CHECK(Utils::ProtoUtils::date_to_proto(Clock::time_point(Clock::duration(-1)))->date() == -1);
    org::polypheny::prism::ProtoDate encoded;
    encoded.set_date(-1);
    const auto decoded = Utils::ProtoUtils::proto_to_date(encoded);
    CHECK(std::chrono::duration_cast<std::chrono::milliseconds>(decoded.time_since_epoch()).count() == -MILLIS_PER_DAY);
    CHECK(Utils::ProtoUtils::date_to_proto(decoded)->date() == -1);
    if constexpr (std::ratio_less<Clock::duration::period, std::milli>::value) {
        org::polypheny::prism::ProtoTimestamp timestamp;
        timestamp.set_timestamp(std::numeric_limits<int64_t>::max());
        CHECK_THROWS_AS(Utils::ProtoUtils::proto_to_timestamp(timestamp), std::out_of_range);
        encoded.set_date(std::numeric_limits<int64_t>::min());
        CHECK_THROWS_AS(Utils::ProtoUtils::proto_to_date(encoded), std::out_of_range);
    }
    encoded.set_date(std::numeric_limits<int64_t>::max());
    CHECK_THROWS_AS(Utils::ProtoUtils::proto_to_date(encoded), std::out_of_range);
}
