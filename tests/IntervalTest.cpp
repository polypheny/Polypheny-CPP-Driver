#include <catch2/catch_all.hpp>
#include <sstream>
#include "Interval.h"
#include "protointerface.pb.h"

using namespace Types;

TEST_CASE("Interval Constructor from ProtoInterval", "[Interval]") {
    org::polypheny::prism::ProtoInterval proto_interval;
    proto_interval.set_months(5);
    proto_interval.set_milliseconds(10000);

    Interval interval(proto_interval);

    REQUIRE(interval.get_months() == 5);
    REQUIRE(interval.get_milliseconds() == 10000);
}

TEST_CASE("Interval Constructor from months and milliseconds", "[Interval]") {
    Interval interval(3, 5000);

    REQUIRE(interval.get_months() == 3);
    REQUIRE(interval.get_milliseconds() == 5000);
}

TEST_CASE("Interval Get months and milliseconds", "[Interval]") {
    Interval interval(7, 15000);

    REQUIRE(interval.get_months() == 7);
    REQUIRE(interval.get_milliseconds() == 15000);
}

TEST_CASE("Interval Equality check", "[Interval]") {
    Interval interval1(6, 20000);
    Interval interval2(6, 20000);
    Interval interval3(6, 30000);

    REQUIRE(interval1.equals(interval2));
    REQUIRE_FALSE(interval1.equals(interval3));
}

TEST_CASE("Interval Stream output with plural months", "[Interval]") {
    Interval interval(4, 1);
    std::ostringstream os;
    os << interval;

    REQUIRE(os.str() == "4 months 1 millisecond");
}

TEST_CASE("Interval Stream output with plural milliseconds", "[Interval]") {
    Interval interval(1, 100);
    std::ostringstream os;
    os << interval;

    REQUIRE(os.str() == "1 month 100 milliseconds");
}

TEST_CASE("Interval Stream output with plurals on zero values", "[Interval]") {
    Interval interval(0, 0);
    std::ostringstream os;
    os << interval;

    REQUIRE(os.str() == "0 months 0 milliseconds");
}

TEST_CASE("Interval Serialization", "[Interval]") {
    Interval interval(2, 4000);
    auto proto_interval = interval.serialize();

    REQUIRE(proto_interval->months() == 2);
    REQUIRE(proto_interval->milliseconds() == 4000);
}
