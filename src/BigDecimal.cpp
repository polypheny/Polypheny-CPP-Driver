#include "BigDecimal.h"
#include "value.pb.h"
#include "ProtoUtils.h"

namespace Types {

    BigDecimal::BigDecimal(const org::polypheny::prism::ProtoBigDecimal &value) : unscaled_value(
            Utils::ProtoUtils::string_to_vector(value.unscaled_value())),
                                                                                  scale(value.scale()) {
        {}
    }

    BigDecimal::BigDecimal(uint32_t scale, const std::vector<uint8_t> unscaled_value)
            : scale(scale), unscaled_value(unscaled_value) {}

    uint32_t BigDecimal::get_scale() const {
        return scale;
    }

    const std::vector<uint8_t> &BigDecimal::get_unscaled_value() const {
        return unscaled_value;
    }

    std::unique_ptr<org::polypheny::prism::ProtoBigDecimal> BigDecimal::serialize() const {
        auto proto_big_decimal = std::make_unique<org::polypheny::prism::ProtoBigDecimal>();
        *proto_big_decimal->mutable_unscaled_value() = Utils::ProtoUtils::vector_to_string(unscaled_value);
        proto_big_decimal->set_scale(scale);
        return proto_big_decimal;
    }
} // Types