#include <catch2/catch_all.hpp>
#include "src/types/TypedValue.h"
#include "src/types/Interval.h"
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
    std::string value = "1234.56";

    mpf_class big_decimal(value);
    Types::TypedValue big_decimal_value(big_decimal);

    mpf_class big_decimal_2 = big_decimal_value.as_big_decimal();
    REQUIRE(big_decimal_2 == big_decimal);
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
    std::unordered_map<std::string, Types::TypedValue> document;
    document["key1"] = Types::TypedValue(42);
    document["key2"] = Types::TypedValue(std::string("value"));
    REQUIRE(document.size() == 2);

    Types::TypedValue document_value(document);

    std::unordered_map<std::string, Types::TypedValue> document_2 = document_value.as_document();
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
    std::string value = "1234.56";
    mpf_class big_decimal(value);
    Types::TypedValue big_decimal_value(big_decimal);
    Types::TypedValue original(big_decimal);
    Types::TypedValue copy(original);
    REQUIRE(copy.as_big_decimal() == original.as_big_decimal());
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
    std::unordered_map<std::string, Types::TypedValue>  document;
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

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_boolean() == true);


}

TEST_CASE("TypedValue move constructor from int32_t", "[TypedValue]") {
    Types::TypedValue original(42);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_int32_t() == 42);


}

TEST_CASE("TypedValue move constructor from int64_t", "[TypedValue]") {
    Types::TypedValue original(int64_t(42000000000));

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_int64_t() == 42000000000);


}

TEST_CASE("TypedValue move constructor from BigDecimal", "[TypedValue]") {
    std::string value = "1234.56";
    mpf_class big_decimal(value);
    Types::TypedValue big_decimal_value(big_decimal);
    Types::TypedValue original(big_decimal);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_big_decimal() == big_decimal);


}

TEST_CASE("TypedValue move constructor from float", "[TypedValue]") {
    Types::TypedValue original(4.2f);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE_THAT(moved.as_float(), Catch::Matchers::WithinAbs(4.2f, 10E-8));


}

TEST_CASE("TypedValue move constructor from double", "[TypedValue]") {
    Types::TypedValue original(4.2);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE_THAT(moved.as_double(), Catch::Matchers::WithinAbs(4.2, 10E-8));


}

TEST_CASE("TypedValue move constructor from date", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue original(now);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_date() == now);


}

TEST_CASE("TypedValue move constructor from time", "[TypedValue]") {
    std::chrono::milliseconds millis(12345);
    Types::TypedValue original(millis);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_time() == millis);


}

TEST_CASE("TypedValue move constructor from timestamp", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue original(now, true);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_timestamp() == now);


}

TEST_CASE("TypedValue move constructor from Interval", "[TypedValue]") {
    Types::Interval interval(5, 20);
    Types::TypedValue original(interval);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_interval().get_milliseconds() == 20);
    REQUIRE(moved.as_interval().get_months() == 5);


}

TEST_CASE("TypedValue move constructor from Document", "[TypedValue]") {
    std::unordered_map<std::string, Types::TypedValue> document;
    document["key1"] = Types::TypedValue(42);
    document["key2"] = Types::TypedValue(std::string("value"));
    Types::TypedValue original(document);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_document().size() == 2);
    REQUIRE(moved.as_document()["key1"].as_int32_t() == 42);
    REQUIRE(moved.as_document()["key2"].as_string() == "value");


}

TEST_CASE("TypedValue move constructor from null", "[TypedValue]") {
    Types::TypedValue original;

    REQUIRE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.is_null());

    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move constructor from list", "[TypedValue]") {
    std::list<Types::TypedValue> list = {Types::TypedValue(1), Types::TypedValue(2)};
    Types::TypedValue original(list);

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


}

TEST_CASE("TypedValue move constructor from string", "[TypedValue]") {
    std::string str = "test";
    Types::TypedValue original(str);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_string() == str);


}

TEST_CASE("TypedValue move constructor from bytes", "[TypedValue]") {
    std::vector<uint8_t> bytes = {'a', 'b', 'c'};
    Types::TypedValue original(bytes);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved(std::move(original));
    REQUIRE(moved.as_bytes() == bytes);


}

TEST_CASE("TypedValue move assignment operator from boolean", "[TypedValue]") {
    Types::TypedValue original(true);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_boolean() == true);


}

TEST_CASE("TypedValue move assignment operator from int32_t", "[TypedValue]") {
    Types::TypedValue original(42);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_int32_t() == 42);


}

TEST_CASE("TypedValue move assignment operator from int64_t", "[TypedValue]") {
    Types::TypedValue original(int64_t(42000000000));

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_int64_t() == 42000000000);


}

TEST_CASE("TypedValue move assignment operator from BigDecimal", "[TypedValue]") {
    std::string value = "1234.56";
    mpf_class big_decimal(value);
    Types::TypedValue big_decimal_value(big_decimal);
    Types::TypedValue original(big_decimal);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_big_decimal() == big_decimal);


}

TEST_CASE("TypedValue move assignment operator from float", "[TypedValue]") {
    Types::TypedValue original(4.2f);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE_THAT(moved.as_float(), Catch::Matchers::WithinAbs(4.2f, 10E-8));


}

TEST_CASE("TypedValue move assignment operator from double", "[TypedValue]") {
    Types::TypedValue original(4.2);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE_THAT(moved.as_double(), Catch::Matchers::WithinAbs(4.2, 10E-8));


}

TEST_CASE("TypedValue move assignment operator from date", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue original(now);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_date() == now);


}

TEST_CASE("TypedValue move assignment operator from time", "[TypedValue]") {
    std::chrono::milliseconds millis(12345);
    Types::TypedValue original(millis);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_time() == millis);


}

TEST_CASE("TypedValue move assignment operator from timestamp", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue original(now, true);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_timestamp() == now);


}

TEST_CASE("TypedValue move assignment operator from Interval", "[TypedValue]") {
    Types::Interval interval(5, 20);
    Types::TypedValue original(interval);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_interval().get_milliseconds() == 20);
    REQUIRE(moved.as_interval().get_months() == 5);


}

TEST_CASE("TypedValue move assignment operator from Document", "[TypedValue]") {
    std::unordered_map<std::string, Types::TypedValue> document;
    document["key1"] = Types::TypedValue(42);
    document["key2"] = Types::TypedValue(std::string("value"));
    Types::TypedValue original(document);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_document().size() == 2);
    REQUIRE(moved.as_document()["key1"].as_int32_t() == 42);
    REQUIRE(moved.as_document()["key2"].as_string() == "value");


}

TEST_CASE("TypedValue move assignment operator from null", "[TypedValue]") {
    Types::TypedValue original;

    REQUIRE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.is_null() == true);

    REQUIRE(original.is_null());
}

TEST_CASE("TypedValue move assignment operator from list", "[TypedValue]") {
    std::list<Types::TypedValue> list = {Types::TypedValue(1), Types::TypedValue(2)};
    Types::TypedValue original(list);

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


}

TEST_CASE("TypedValue move assignment operator from string", "[TypedValue]") {
    std::string str = "test";
    Types::TypedValue original(str);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_string() == str);


}

TEST_CASE("TypedValue move assignment operator from bytes", "[TypedValue]") {
    std::vector<uint8_t> bytes = {'a', 'b', 'c'};
    Types::TypedValue original(bytes);

    REQUIRE_FALSE(original.is_null());
    Types::TypedValue moved;
    moved = std::move(original);
    REQUIRE(moved.as_bytes() == bytes);


}

// Copy Assignment Operator Tests

TEST_CASE("TypedValue copy assignment operator from boolean", "[TypedValue]") {
    Types::TypedValue original(true);
    Types::TypedValue copy;

    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_boolean() == original.as_boolean());
}

TEST_CASE("TypedValue copy assignment operator from int32_t", "[TypedValue]") {
    Types::TypedValue original(42);
    Types::TypedValue copy;

    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_int32_t() == original.as_int32_t());
}

TEST_CASE("TypedValue copy assignment operator from int64_t", "[TypedValue]") {
    Types::TypedValue original(int64_t(42000000000));
    Types::TypedValue copy;

    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_int64_t() == original.as_int64_t());
}

TEST_CASE("TypedValue copy assignment operator from BigDecimal", "[TypedValue]") {
    std::string value = "1234.56";
    mpf_class big_decimal(value);
    Types::TypedValue big_decimal_value(big_decimal);
    Types::TypedValue original(big_decimal);
    Types::TypedValue copy;

    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_big_decimal() == original.as_big_decimal());
}

TEST_CASE("TypedValue copy assignment operator from float", "[TypedValue]") {
    Types::TypedValue original(4.2f);
    Types::TypedValue copy;

    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE_THAT(copy.as_float(), Catch::Matchers::WithinAbs(original.as_float(), 10E-8));
}

TEST_CASE("TypedValue copy assignment operator from double", "[TypedValue]") {
    Types::TypedValue original(4.2);
    Types::TypedValue copy;

    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE_THAT(copy.as_double(), Catch::Matchers::WithinAbs(original.as_double(), 10E-8));
}

TEST_CASE("TypedValue copy assignment operator from date", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue original(now);
    Types::TypedValue copy;

    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_date() == original.as_date());
}

TEST_CASE("TypedValue copy assignment operator from time", "[TypedValue]") {
    std::chrono::milliseconds millis(12345);
    Types::TypedValue original(millis);
    Types::TypedValue copy;

    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_time() == original.as_time());
}

TEST_CASE("TypedValue copy assignment operator from timestamp", "[TypedValue]") {
    auto now = std::chrono::system_clock::now();
    Types::TypedValue original(now, true);
    Types::TypedValue copy;

    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_timestamp() == original.as_timestamp());
}

TEST_CASE("TypedValue copy assignment operator from Interval", "[TypedValue]") {
    Types::Interval interval(5, 20);
    Types::TypedValue original(interval);
    Types::TypedValue copy;

    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_interval().get_milliseconds() == original.as_interval().get_milliseconds());
    REQUIRE(copy.as_interval().get_months() == original.as_interval().get_months());
}

TEST_CASE("TypedValue copy assignment operator from Document", "[TypedValue]") {
    std::unordered_map<std::string, Types::TypedValue> document;
    document["key1"] = Types::TypedValue(42);
    document["key2"] = Types::TypedValue(std::string("value"));
    Types::TypedValue original(document);
    Types::TypedValue copy;

    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_document().size() == original.as_document().size());
    REQUIRE(copy.as_document()["key1"].as_int32_t() == original.as_document()["key1"].as_int32_t());
    REQUIRE(copy.as_document()["key2"].as_string() == original.as_document()["key2"].as_string());
}

TEST_CASE("TypedValue copy assignment operator from null", "[TypedValue]") {
    Types::TypedValue original;
    Types::TypedValue copy;

    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.is_null() == original.is_null());
}

TEST_CASE("TypedValue copy assignment operator from list", "[TypedValue]") {
    std::list<Types::TypedValue> list = {Types::TypedValue(1), Types::TypedValue(2)};
    Types::TypedValue original(list);
    Types::TypedValue copy;

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

    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_string() == original.as_string());
}

TEST_CASE("TypedValue copy assignment operator from bytes", "[TypedValue]") {
    std::vector<uint8_t> bytes = {'a', 'b', 'c'};
    Types::TypedValue original(bytes);
    Types::TypedValue copy;

    REQUIRE(copy.is_null());
    copy = original;
    REQUIRE(copy.as_bytes() == original.as_bytes());
}

TEST_CASE("TypedValue print boolean", "[TypedValue]") {
    Types::TypedValue boolean_value(true);
    std::ostringstream oss;
    oss << boolean_value;
    REQUIRE(oss.str() == "true");
}

TEST_CASE("TypedValue print int32_t", "[TypedValue]") {
    Types::TypedValue int_value(42);
    std::ostringstream oss;
    oss << int_value;
    REQUIRE(oss.str() == "42");
}

TEST_CASE("TypedValue print int64_t", "[TypedValue]") {
    Types::TypedValue long_value(int64_t(42000000000));
    std::ostringstream oss;
    oss << long_value;
    REQUIRE(oss.str() == "42000000000");
}

TEST_CASE("TypedValue print BigDecimal", "[TypedValue]") {
    std::string value = "1234.56";
    mpf_class big_decimal(value);
    Types::TypedValue big_decimal_value(big_decimal);
    std::ostringstream oss;
    oss << big_decimal_value;
    REQUIRE(oss.str() == value);
}

TEST_CASE("TypedValue print float", "[TypedValue]") {
    Types::TypedValue float_value(4.2f);
    std::ostringstream oss;
    oss << float_value;
    REQUIRE(oss.str() == "4.2");
}

TEST_CASE("TypedValue print double", "[TypedValue]") {
    Types::TypedValue double_value(4.2);
    std::ostringstream oss;
    oss << double_value;
    REQUIRE(oss.str() == "4.2");
}

TEST_CASE("TypedValue print date", "[TypedValue]") {
    auto hardcoded_time = std::chrono::system_clock::time_point(std::chrono::seconds(1622505600)); // 2021-06-01 00:00:00 UTC
    Types::TypedValue date_value(hardcoded_time);
    std::ostringstream oss;
    oss << date_value;
    std::string expected_date = "2021-06-01";
    REQUIRE(oss.str() == expected_date);
}

TEST_CASE("TypedValue print time", "[TypedValue]") {
    std::chrono::milliseconds millis(12345);
    Types::TypedValue time_value(millis);
    std::ostringstream oss;
    oss << time_value;
    REQUIRE(oss.str() == "00:00:12.345");
}

TEST_CASE("TypedValue print timestamp", "[TypedValue]") {
    auto hardcoded_time = std::chrono::system_clock::time_point(std::chrono::milliseconds(1654041600000)); // 2022-06-01 00:00:00 UTC
    Types::TypedValue timestamp_value(hardcoded_time, true);
    std::ostringstream oss;
    oss << timestamp_value;

    std::string expected_timestamp = "2022-06-01 00:00:00.000";
    REQUIRE(oss.str() == expected_timestamp);
}

TEST_CASE("TypedValue print interval", "[TypedValue]") {
    Types::Interval interval(5, 20);
    Types::TypedValue interval_value(interval);
    std::ostringstream oss;
    oss << interval_value;
    REQUIRE(oss.str() == "5 months 20 milliseconds"); // Example output format
}

TEST_CASE("TypedValue print document", "[TypedValue]") {
    std::unordered_map<std::string, Types::TypedValue> document;
    document["key1"] = Types::TypedValue(42);
    document["key2"] = Types::TypedValue(std::string("value"));
    Types::TypedValue document_value(document);
    std::ostringstream oss;
    oss << document_value;
    REQUIRE((oss.str() == "{\"key1\": 42, \"key2\": \"value\"}" || oss.str() == "{\"key2\": \"value\", \"key1\": 42}"));
}

TEST_CASE("TypedValue print null", "[TypedValue]") {
    Types::TypedValue null_value;
    std::ostringstream oss;
    oss << null_value;
    REQUIRE(oss.str() == "null");
}

TEST_CASE("TypedValue print list", "[TypedValue]") {
    std::list<Types::TypedValue> list = {Types::TypedValue(1), Types::TypedValue(2)};
    Types::TypedValue list_value(list);
    std::ostringstream oss;
    oss << list_value;
    REQUIRE(oss.str() == "[1, 2]");
}

TEST_CASE("TypedValue print string", "[TypedValue]") {
    std::string str = "test";
    Types::TypedValue string_value(str);
    std::ostringstream oss;
    oss << string_value;
    REQUIRE(oss.str() == "\"test\"");
}

TEST_CASE("TypedValue print bytes", "[TypedValue]") {
    std::vector<uint8_t> bytes = {'a', 'b', 'c'};
    Types::TypedValue bytes_value(bytes);
    std::ostringstream oss;
    oss << bytes_value;
    REQUIRE(oss.str() == "616263"); // Hex representation of 'a', 'b', 'c'
}
