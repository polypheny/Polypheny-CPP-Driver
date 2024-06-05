#include <catch2/catch_all.hpp>
#include "BigDecimal.h"
#include "value.pb.h"
#include "ProtoUtils.h"

TEST_CASE("BigDecimal constructor from ProtoBigDecimal") {
    org::polypheny::prism::ProtoBigDecimal proto;
    proto.set_unscaled_value("123456789");
    proto.set_scale(10);

    Types::BigDecimal bigDecimal(proto);

    REQUIRE(bigDecimal.get_scale() == 10);
    REQUIRE(bigDecimal.get_unscaled_value() == Utils::ProtoUtils::string_to_vector("123456789"));
}

TEST_CASE("BigDecimal constructor with scale and unscaled_value") {
    uint32_t scale = 5;
    std::vector<uint8_t> unscaled_value = {1, 2, 3, 4, 5};

    Types::BigDecimal bigDecimal(scale, unscaled_value);

    REQUIRE(bigDecimal.get_scale() == scale);
    REQUIRE(bigDecimal.get_unscaled_value() == unscaled_value);
}

TEST_CASE("BigDecimal serialize to ProtoBigDecimal") {
    uint32_t scale = 7;
    std::vector<uint8_t> unscaled_value = {9, 8, 7, 6, 5};

    Types::BigDecimal bigDecimal(scale, unscaled_value);

    auto proto = bigDecimal.serialize();

    REQUIRE(proto->scale() == scale);
    REQUIRE(proto->unscaled_value() == Utils::ProtoUtils::vector_to_string(unscaled_value));
}

TEST_CASE("BigDecimal getters") {
    uint32_t scale = 3;
    std::vector<uint8_t> unscaled_value = {4, 3, 2, 1};

    Types::BigDecimal bigDecimal(scale, unscaled_value);

    REQUIRE(bigDecimal.get_scale() == scale);
    REQUIRE(bigDecimal.get_unscaled_value() == unscaled_value);
}
