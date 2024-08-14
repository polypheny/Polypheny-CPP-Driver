//
// Created by tobi on 13.08.24.
//

#include <catch2/catch_all.hpp>
#include <PolyphenyCPPDriver.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <memory>

using namespace std;

const std::string FILE_DROP_IF_STATEMENT = "DROP TABLE IF EXISTS file_table";
const std::string FILE_CREATE_STATEMENT = "CREATE TABLE file_table (id INT PRIMARY KEY, data FILE)";
const std::string FILE_INSERT_STATEMENT = "INSERT INTO file_table (id, data) VALUES (?, ?)";
const std::string FILE_QUERY = "SELECT data FROM file_table WHERE id = 1";

const std::string STRING_DROP_IF_STATEMENT = "DROP TABLE IF EXISTS string_table";
const std::string STRING_CREATE_STATEMENT = "CREATE TABLE string_table (id INT PRIMARY KEY, data VARCHAR)";
const std::string STRING_INSERT_STATEMENT = "INSERT INTO string_table (id, data) VALUES (?, ?)";
const std::string STRING_QUERY = "SELECT data FROM string_table WHERE id = 1";

std::vector<uint8_t> createTestData() {
    const int TOTAL_BYTES = 300 * 1024 * 1024;
    std::vector<uint8_t> data(TOTAL_BYTES);
    for (int i = 0; i < TOTAL_BYTES; i += 4) {
        int value = i / 4;
        std::memcpy(&data[i], &value, sizeof(value));
    }
    return data;
}

std::string createTestStringData() {
    const int TOTAL_CHARS = 300 * 1024 * 1024;
    return std::string(TOTAL_CHARS, 'A');
}

std::vector<uint8_t> collectStream(std::streambuf *buf) {
    std::vector<uint8_t> buffer;
    buffer.reserve(300 * 1024 * 1024);
    std::array<char, 1024> frame;

    std::istream inputStream(buf);

    while (inputStream.read(frame.data(), frame.size()) || inputStream.gcount()) {
        buffer.insert(buffer.end(), frame.data(), frame.data() + inputStream.gcount());
    }

    return buffer;
}

/*
TEST_CASE("Simple File Streaming Test", "[StreamingTest]") {

    Connection::Connection db_conn("localhost", "pa", "");
    Connection::Cursor cursor(db_conn);

    cursor.execute("sql", FILE_DROP_IF_STATEMENT, "public");
    cursor.execute("sql", FILE_CREATE_STATEMENT, "public");

    cursor.prepare("sql", FILE_INSERT_STATEMENT, "public");

    auto expected = createTestData();
    std::vector<Types::TypedValue> parameters = {
            Types::TypedValue(1),
            Types::TypedValue(expected)
    };

    std::unique_ptr<Results::Result> result = cursor.execute_prepared(parameters);
    result->unwrap<Results::ScalarResult>();

    std::unique_ptr<Results::Result> result2 = cursor.execute("sql", FILE_QUERY, "public");
    auto *relational_result = result2->unwrap<Results::RelationalResult>();

    auto row = relational_result->begin();
    if (row != relational_result->end()) {
        auto file = (*row)["data"].as_file();
        auto received = collectStream(&file);

        REQUIRE(expected == received);
    } else {
        FAIL("No rows returned from query");
    }
}


TEST_CASE("Simple File Streaming Test 2", "[StreamingTest]") {
    try {
        Connection::Connection db_conn("localhost", "pa", "");
        Connection::Cursor cursor(db_conn);

        cursor.execute("sql", FILE_DROP_IF_STATEMENT, "public");
        cursor.execute("sql", FILE_CREATE_STATEMENT, "public");

        cursor.prepare("sql", FILE_INSERT_STATEMENT, "public");

        auto expected = createTestData();
        std::istringstream istream(std::string(expected.begin(), expected.end()));
        std::vector<Types::TypedValue> parameters = {
                Types::TypedValue(1),
                Types::TypedValue(Types::File(istream))
        };

        std::unique_ptr<Results::Result> result = cursor.execute_prepared(parameters);
        result->unwrap<Results::ScalarResult>();

        std::unique_ptr<Results::Result> result2 = cursor.execute("sql", FILE_QUERY, "public");
        auto* relational_result = result2->unwrap<Results::RelationalResult>();

        auto row = relational_result->begin();
        auto file = (*row)["data"].as_file();
        auto received = collectStream(&file);

        REQUIRE(expected == received);
    } catch (const std::exception& e) {
        FAIL(e.what());
    }
}

*/

TEST_CASE("Simple String Streaming Test", "[StreamingTest]") {

    Connection::Connection db_conn("localhost", "pa", "");
    Connection::Cursor cursor(db_conn);

    cursor.execute("sql", STRING_DROP_IF_STATEMENT, "public");
    cursor.execute("sql", STRING_CREATE_STATEMENT, "public");

    cursor.prepare("sql", STRING_INSERT_STATEMENT, "public");

    std::string expected = createTestStringData();
    std::vector<Types::TypedValue> parameters = {
            Types::TypedValue(1),
            Types::TypedValue(expected)
    };

    std::unique_ptr<Results::Result> result = cursor.execute_prepared(parameters);
    result->

            unwrap<Results::ScalarResult>();

    std::unique_ptr<Results::Result> result2 = cursor.execute("sql", STRING_QUERY, "public");
    auto *relational_result = result2->unwrap<Results::RelationalResult>();

    auto row = relational_result->begin();
    auto received = (*row)["data"].as_string();

    REQUIRE(expected == received);

}

