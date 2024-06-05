#include <catch2/catch_all.hpp>
#include "ConnectionClosedError.h"

TEST_CASE("ConnectionClosedError message test") {
    std::string errorMessage = "Connection closed unexpectedly";
    Errors::ConnectionClosedError error(errorMessage);

    REQUIRE(std::string(error.what()) == errorMessage);
}

TEST_CASE("ConnectionClosedError throw and catch test") {
    std::string errorMessage = "Connection closed unexpectedly";

    try {
        throw Errors::ConnectionClosedError(errorMessage);
    } catch (const Errors::ConnectionClosedError &e) {
        REQUIRE(std::string(e.what()) == errorMessage);
    }
}
