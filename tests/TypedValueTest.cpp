#include <catch2/catch_all.hpp>
#include "TypedValue.h"
#include "BigDecimal.h"
#include "Document.h"
#include "Interval.h"
#include "value.pb.h"

TEST_CASE("TypedValue construction from boolean", "[TypedValue]") {
    Types::TypedValue boolean_value(true);
    REQUIRE(boolean_value.as_boolean() == true);
}

TEST_CASE("TypedValue construction from int32_t", "[TypedValue]") {
    Types::TypedValue int_value(42);
    REQUIRE(int_value.as_int32_t() == 42);
}

TEST_CASE("TypedValue construction from int64_t", "[TypedValue]") {
    Types::TypedValue long_value(int64_t(42000000000));
    REQUIRE(long_value.as_int64_t() == 42000000000);
}


TEST_CASE("TypedValue construction from BigDecimal", "[TypedValue]") {
    uint32_t scale = 5;
    std::vector<uint8_t> unscaled_value = {1, 2, 3, 4, 5};

    Types::BigDecimal big_decimal(scale, unscaled_value);
    Types::TypedValue big_decimal_value(big_decimal);

    Types::BigDecimal big_decimal_2 = big_decimal_value.as_big_decimal();
    REQUIRE(big_decimal_2.get_scale() == big_decimal.get_scale());
    REQUIRE(big_decimal_2.get_unscaled_value() == big_decimal.get_unscaled_value());
}

TEST_CASE("TypedValue construction from float", "[TypedValue]") {
    Types::TypedValue float_value(4.2f);
    REQUIRE_THAT(float_value.as_float(), Catch::Matchers::WithinAbs(4.2f, 10E-8));
}

TEST_CASE("TypedValue construction from double", "[TypedValue]") {
    Types::TypedValue double_value(4.2);
    REQUIRE_THAT(double_value.as_double(), Catch::Matchers::WithinAbs(4.2, 10E-8));
}

TEST_CASE("TypedValue construction from date", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue date_value(now);
    REQUIRE(date_value.as_date() == now);
}

TEST_CASE("TypedValue construction from time", "[TypedValue]") {
    std::chrono::milliseconds millis(12345);
    Types::TypedValue time_value(millis);
    REQUIRE(time_value.as_time() == millis);
}

TEST_CASE("TypedValue construction from timestamp", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue timestamp_value(now, true);
    REQUIRE(timestamp_value.as_timestamp() == now);
}

TEST_CASE("TypedValue construction from Interval", "[TypedValue]") {
    Types::Interval interval(5, 20);
    Types::TypedValue interval_value(interval);
    Types::Interval interval_2 = interval_value.as_interval();
    REQUIRE(interval_2.get_milliseconds() == interval.get_milliseconds());
    REQUIRE(interval_2.get_months() == interval.get_months());
}


TEST_CASE("TypedValue construction from Document", "[TypedValue]") {
    Types::Document document;
    document["key1"] = Types::TypedValue(42);
    document["key2"] = Types::TypedValue(std::string("value"));
    REQUIRE(document.size() == 2);

    Types::TypedValue document_value(document);

    Types::Document document_2 = document_value.as_document();
    REQUIRE(document_2.size() == 2);
    REQUIRE(document_2["key1"].as_int32_t() == 42);
    REQUIRE(document_2["key2"].as_string() == "value");
}

TEST_CASE("TypedValue construction from null", "[TypedValue]") {
    Types::TypedValue null_value;
    REQUIRE(null_value.is_null() == true);
}

TEST_CASE("TypedValue construction from list", "[TypedValue]") {
    std::list<Types::TypedValue> list = {Types::TypedValue(1), Types::TypedValue(2)};
    Types::TypedValue list_value(list);
    std::list<Types::TypedValue> list_2 = list_value.as_list();
    auto it = list_2.begin();
    REQUIRE(it->as_int32_t() == 1);
    ++it;
    REQUIRE(it->as_int32_t() == 2);
    ++it;
    REQUIRE(it == list_2.end());
}

TEST_CASE("TypedValue construction from string", "[TypedValue]") {
    std::string str = "test";
    Types::TypedValue string_value(str);
    REQUIRE(string_value.as_string() == str);
}

TEST_CASE("TypedValue construction from bytes", "[TypedValue]") {
    std::vector<uint8_t> bytes = {'a', 'b', 'c'};
    Types::TypedValue bytes_value(bytes);
    REQUIRE(bytes_value.as_bytes() == bytes);
}

TEST_CASE("TypedValue as_boolean", "[TypedValue]") {
    Types::TypedValue boolean_value(true);
    REQUIRE(boolean_value.as_boolean() == true);
}

TEST_CASE("TypedValue as_int32_t", "[TypedValue]") {
    Types::TypedValue int_value(42);
    REQUIRE(int_value.as_int32_t() == 42);
}

TEST_CASE("TypedValue as_int64_t", "[TypedValue]") {
    Types::TypedValue long_value(int64_t(42000000000));
    REQUIRE(long_value.as_int64_t() == 42000000000);
}

TEST_CASE("TypedValue as_float", "[TypedValue]") {
    Types::TypedValue float_value(4.2f);
    REQUIRE_THAT(float_value.as_float(), Catch::Matchers::WithinAbs(4.2f, 10E-8));
}

TEST_CASE("TypedValue as_double", "[TypedValue]") {
    Types::TypedValue double_value(4.2);
    REQUIRE_THAT(double_value.as_double(), Catch::Matchers::WithinAbs(4.2f, 10E-8));
}

TEST_CASE("TypedValue as_string", "[TypedValue]") {
    std::string str = "test";
    Types::TypedValue string_value(str);
    REQUIRE(string_value.as_string() == str);
}

TEST_CASE("TypedValue as_bytes", "[TypedValue]") {
    std::vector<uint8_t> bytes = {'a', 'b', 'c'};
    Types::TypedValue bytes_value(bytes);
    REQUIRE(bytes_value.as_bytes() == bytes);
}
