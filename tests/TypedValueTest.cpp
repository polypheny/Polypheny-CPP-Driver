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
    REQUIRE_THAT(float_value.as_float(), Catch::Matchers::WithinAbs(4.2f, 10E-6));
}

TEST_CASE("TypedValue as_double", "[TypedValue]") {
    Types::TypedValue double_value(4.2);
    REQUIRE_THAT(double_value.as_double(), Catch::Matchers::WithinAbs(4.2f, 10E-6));
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

TEST_CASE("TypedValue copy constructor from boolean", "[TypedValue]") {
    Types::TypedValue original(true);
    Types::TypedValue copy(original);
    REQUIRE(copy.as_boolean() == original.as_boolean());
}

TEST_CASE("TypedValue copy constructor from int32_t", "[TypedValue]") {
    Types::TypedValue original(42);
    Types::TypedValue copy(original);
    REQUIRE(copy.as_int32_t() == original.as_int32_t());
}

TEST_CASE("TypedValue copy constructor from int64_t", "[TypedValue]") {
    Types::TypedValue original(int64_t(42000000000));
    Types::TypedValue copy(original);
    REQUIRE(copy.as_int64_t() == original.as_int64_t());
}

TEST_CASE("TypedValue copy constructor from BigDecimal", "[TypedValue]") {
    uint32_t scale = 5;
    std::vector<uint8_t> unscaled_value = {1, 2, 3, 4, 5};
    Types::BigDecimal big_decimal(scale, unscaled_value);
    Types::TypedValue original(big_decimal);
    Types::TypedValue copy(original);
    REQUIRE(copy.as_big_decimal().get_scale() == original.as_big_decimal().get_scale());
    REQUIRE(copy.as_big_decimal().get_unscaled_value() == original.as_big_decimal().get_unscaled_value());
}

TEST_CASE("TypedValue copy constructor from float", "[TypedValue]") {
    Types::TypedValue original(4.2f);
    Types::TypedValue copy(original);
    REQUIRE_THAT(copy.as_float(), Catch::Matchers::WithinAbs(original.as_float(), 10E-6));
}

TEST_CASE("TypedValue copy constructor from double", "[TypedValue]") {
    Types::TypedValue original(4.2);
    Types::TypedValue copy(original);
    REQUIRE_THAT(copy.as_double(), Catch::Matchers::WithinAbs(original.as_double(), 10E-6));
}

TEST_CASE("TypedValue copy constructor from date", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue original(now);
    Types::TypedValue copy(original);
    REQUIRE(copy.as_date() == original.as_date());
}

TEST_CASE("TypedValue copy constructor from time", "[TypedValue]") {
    std::chrono::milliseconds millis(12345);
    Types::TypedValue original(millis);
    Types::TypedValue copy(original);
    REQUIRE(copy.as_time() == original.as_time());
}

TEST_CASE("TypedValue copy constructor from timestamp", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue original(now, true);
    Types::TypedValue copy(original);
    REQUIRE(copy.as_timestamp() == original.as_timestamp());
}

TEST_CASE("TypedValue copy constructor from Interval", "[TypedValue]") {
    Types::Interval interval(5, 20);
    Types::TypedValue original(interval);
    Types::TypedValue copy(original);
    REQUIRE(copy.as_interval().get_milliseconds() == original.as_interval().get_milliseconds());
    REQUIRE(copy.as_interval().get_months() == original.as_interval().get_months());
}

TEST_CASE("TypedValue copy constructor from Document", "[TypedValue]") {
    Types::Document document;
    document["key1"] = Types::TypedValue(42);
    document["key2"] = Types::TypedValue(std::string("value"));
    Types::TypedValue original(document);
    Types::TypedValue copy(original);
    REQUIRE(copy.as_document().size() == original.as_document().size());
    REQUIRE(copy.as_document()["key1"].as_int32_t() == original.as_document()["key1"].as_int32_t());
    REQUIRE(copy.as_document()["key2"].as_string() == original.as_document()["key2"].as_string());
}

TEST_CASE("TypedValue copy constructor from null", "[TypedValue]") {
    Types::TypedValue original;
    Types::TypedValue copy(original);
    REQUIRE(copy.is_null() == original.is_null());
}

TEST_CASE("TypedValue copy constructor from list", "[TypedValue]") {
    std::list<Types::TypedValue> list = {Types::TypedValue(1), Types::TypedValue(2)};
    Types::TypedValue original(list);
    Types::TypedValue copy(original);
    auto original_list = original.as_list();
    auto copy_list = copy.as_list();
    REQUIRE(copy_list.size() == original_list.size());
    auto it1 = original_list.begin();
    auto it2 = copy_list.begin();
    while (it1 != original_list.end() && it2 != copy_list.end()) {
        REQUIRE(it2->as_int32_t() == it1->as_int32_t());
        ++it1;
        ++it2;
    }
}

TEST_CASE("TypedValue copy constructor from string", "[TypedValue]") {
    std::string str = "test";
    Types::TypedValue original(str);
    Types::TypedValue copy(original);
    REQUIRE(copy.as_string() == original.as_string());
}

TEST_CASE("TypedValue copy constructor from bytes", "[TypedValue]") {
    std::vector<uint8_t> bytes = {'a', 'b', 'c'};
    Types::TypedValue original(bytes);
    Types::TypedValue copy(original);
    REQUIRE(copy.as_bytes() == original.as_bytes());
}

TEST_CASE("TypedValue move constructor from boolean", "[TypedValue]") {
    Types::TypedValue original(true);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_boolean() == true);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from int32_t", "[TypedValue]") {
    Types::TypedValue original(42);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_int32_t() == 42);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from int64_t", "[TypedValue]") {
    Types::TypedValue original(int64_t(42000000000));
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_int64_t() == 42000000000);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from BigDecimal", "[TypedValue]") {
    uint32_t scale = 5;
    std::vector<uint8_t> unscaled_value = {1, 2, 3, 4, 5};
    Types::BigDecimal big_decimal(scale, unscaled_value);
    Types::TypedValue original(big_decimal);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_big_decimal().get_scale() == scale);
    REQUIRE(moved.as_big_decimal().get_unscaled_value() == unscaled_value);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from float", "[TypedValue]") {
    Types::TypedValue original(4.2f);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE_THAT(moved.as_float(), Catch::Matchers::WithinAbs(4.2f, 10E-8));
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from double", "[TypedValue]") {
    Types::TypedValue original(4.2);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE_THAT(moved.as_double(), Catch::Matchers::WithinAbs(4.2, 10E-8));
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from date", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue original(now);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_date() == now);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from time", "[TypedValue]") {
    std::chrono::milliseconds millis(12345);
    Types::TypedValue original(millis);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_time() == millis);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from timestamp", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue original(now, true);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_timestamp() == now);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from Interval", "[TypedValue]") {
    Types::Interval interval(5, 20);
    Types::TypedValue original(interval);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_interval().get_milliseconds() == 20);
    REQUIRE(moved.as_interval().get_months() == 5);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from Document", "[TypedValue]") {
    Types::Document document;
    document["key1"] = Types::TypedValue(42);
    document["key2"] = Types::TypedValue(std::string("value"));
    Types::TypedValue original(document);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_document().size() == 2);
    REQUIRE(moved.as_document()["key1"].as_int32_t() == 42);
    REQUIRE(moved.as_document()["key2"].as_string() == "value");
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from null", "[TypedValue]") {
    Types::TypedValue original;
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.is_null() == true);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from list", "[TypedValue]") {
    std::list<Types::TypedValue> list = {Types::TypedValue(1), Types::TypedValue(2)};
    Types::TypedValue original(list);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    auto moved_list = moved.as_list();
    REQUIRE(moved_list.size() == 2);
    auto it = moved_list.begin();
    REQUIRE(it->as_int32_t() == 1);
    ++it;
    REQUIRE(it->as_int32_t() == 2);
    ++it;
    REQUIRE(it == moved_list.end());
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from string", "[TypedValue]") {
    std::string str = "test";
    Types::TypedValue original(str);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_string() == str);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from bytes", "[TypedValue]") {
    std::vector<uint8_t> bytes = {'a', 'b', 'c'};
    Types::TypedValue original(bytes);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_bytes() == bytes);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from boolean", "[TypedValue]") {
    Types::TypedValue original(true);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_boolean() == true);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from int32_t", "[TypedValue]") {
    Types::TypedValue original(42);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_int32_t() == 42);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from int64_t", "[TypedValue]") {
    Types::TypedValue original(int64_t(42000000000));
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_int64_t() == 42000000000);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from BigDecimal", "[TypedValue]") {
    uint32_t scale = 5;
    std::vector<uint8_t> unscaled_value = {1, 2, 3, 4, 5};
    Types::BigDecimal big_decimal(scale, unscaled_value);
    Types::TypedValue original(big_decimal);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_big_decimal().get_scale() == scale);
    REQUIRE(moved.as_big_decimal().get_unscaled_value() == unscaled_value);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from float", "[TypedValue]") {
    Types::TypedValue original(4.2f);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE_THAT(moved.as_float(), Catch::Matchers::WithinAbs(4.2f, 10E-8));
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from double", "[TypedValue]") {
    Types::TypedValue original(4.2);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE_THAT(moved.as_double(), Catch::Matchers::WithinAbs(4.2, 10E-8));
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from date", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue original(now);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_date() == now);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from time", "[TypedValue]") {
    std::chrono::milliseconds millis(12345);
    Types::TypedValue original(millis);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_time() == millis);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from timestamp", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue original(now, true);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_timestamp() == now);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from Interval", "[TypedValue]") {
    Types::Interval interval(5, 20);
    Types::TypedValue original(interval);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_interval().get_milliseconds() == 20);
    REQUIRE(moved.as_interval().get_months() == 5);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from Document", "[TypedValue]") {
    Types::Document document;
    document["key1"] = Types::TypedValue(42);
    document["key2"] = Types::TypedValue(std::string("value"));
    Types::TypedValue original(document);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_document().size() == 2);
    REQUIRE(moved.as_document()["key1"].as_int32_t() == 42);
    REQUIRE(moved.as_document()["key2"].as_string() == "value");
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from null", "[TypedValue]") {
    Types::TypedValue original;
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.is_null() == true);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from list", "[TypedValue]") {
    std::list<Types::TypedValue> list = {Types::TypedValue(1), Types::TypedValue(2)};
    Types::TypedValue original(list);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    auto moved_list = moved.as_list();
    REQUIRE(moved_list.size() == 2);
    auto it = moved_list.begin();
    REQUIRE(it->as_int32_t() == 1);
    ++it;
    REQUIRE(it->as_int32_t() == 2);
    ++it;
    REQUIRE(it == moved_list.end());
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from string", "[TypedValue]") {
    std::string str = "test";
    Types::TypedValue original(str);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_string() == str);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from bytes", "[TypedValue]") {
    std::vector<uint8_t> bytes = {'a', 'b', 'c'};
    Types::TypedValue original(bytes);
    REQUIRE_FALSE(original.is_moved_from());
    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_bytes() == bytes);
    REQUIRE(original.is_moved_from());
    REQUIRE(original.is_null());
}

// Copy Assignment Operator Tests

TEST_CASE("TypedValue copy assignment operator from boolean", "[TypedValue]") {
    Types::TypedValue original(true);
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_boolean() == original.as_boolean());
}

TEST_CASE("TypedValue copy assignment operator from int32_t", "[TypedValue]") {
    Types::TypedValue original(42);
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_int32_t() == original.as_int32_t());
}

TEST_CASE("TypedValue copy assignment operator from int64_t", "[TypedValue]") {
    Types::TypedValue original(int64_t(42000000000));
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_int64_t() == original.as_int64_t());
}

TEST_CASE("TypedValue copy assignment operator from BigDecimal", "[TypedValue]") {
    uint32_t scale = 5;
    std::vector<uint8_t> unscaled_value = {1, 2, 3, 4, 5};
    Types::BigDecimal big_decimal(scale, unscaled_value);
    Types::TypedValue original(big_decimal);
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_big_decimal().get_scale() == original.as_big_decimal().get_scale());
    REQUIRE(copy.as_big_decimal().get_unscaled_value() == original.as_big_decimal().get_unscaled_value());
}

TEST_CASE("TypedValue copy assignment operator from float", "[TypedValue]") {
    Types::TypedValue original(4.2f);
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE_THAT(copy.as_float(), Catch::Matchers::WithinAbs(original.as_float(), 10E-8));
}

TEST_CASE("TypedValue copy assignment operator from double", "[TypedValue]") {
    Types::TypedValue original(4.2);
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE_THAT(copy.as_double(), Catch::Matchers::WithinAbs(original.as_double(), 10E-8));
}

TEST_CASE("TypedValue copy assignment operator from date", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue original(now);
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_date() == original.as_date());
}

TEST_CASE("TypedValue copy assignment operator from time", "[TypedValue]") {
    std::chrono::milliseconds millis(12345);
    Types::TypedValue original(millis);
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_time() == original.as_time());
}

TEST_CASE("TypedValue copy assignment operator from timestamp", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue original(now, true);
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_timestamp() == original.as_timestamp());
}

TEST_CASE("TypedValue copy assignment operator from Interval", "[TypedValue]") {
    Types::Interval interval(5, 20);
    Types::TypedValue original(interval);
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_interval().get_milliseconds() == original.as_interval().get_milliseconds());
    REQUIRE(copy.as_interval().get_months() == original.as_interval().get_months());
}

TEST_CASE("TypedValue copy assignment operator from Document", "[TypedValue]") {
    Types::Document document;
    document["key1"] = Types::TypedValue(42);
    document["key2"] = Types::TypedValue(std::string("value"));
    Types::TypedValue original(document);
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_document().size() == original.as_document().size());
    REQUIRE(copy.as_document()["key1"].as_int32_t() == original.as_document()["key1"].as_int32_t());
    REQUIRE(copy.as_document()["key2"].as_string() == original.as_document()["key2"].as_string());
}

TEST_CASE("TypedValue copy assignment operator from null", "[TypedValue]") {
    Types::TypedValue original;
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.is_null() == original.is_null());
}

TEST_CASE("TypedValue copy assignment operator from list", "[TypedValue]") {
    std::list<Types::TypedValue> list = {Types::TypedValue(1), Types::TypedValue(2)};
    Types::TypedValue original(list);
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    auto original_list = original.as_list();
    auto copy_list = copy.as_list();
    REQUIRE(copy_list.size() == original_list.size());
    auto it1 = original_list.begin();
    auto it2 = copy_list.begin();
    while (it1 != original_list.end() && it2 != copy_list.end()) {
        REQUIRE(it2->as_int32_t() == it1->as_int32_t());
        ++it1;
        ++it2;
    }
}

TEST_CASE("TypedValue copy assignment operator from string", "[TypedValue]") {
    std::string str = "test";
    Types::TypedValue original(str);
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_string() == original.as_string());
}

TEST_CASE("TypedValue copy assignment operator from bytes", "[TypedValue]") {
    std::vector<uint8_t> bytes = {'a', 'b', 'c'};
    Types::TypedValue original(bytes);
    Types::TypedValue copy;
    REQUIRE_FALSE(copy.is_moved_from());
    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_bytes() == original.as_bytes());
}
