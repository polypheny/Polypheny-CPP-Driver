#include "BigDecimal.h"

namespace Types {
    BigDecimal::BigDecimal(org::polypheny::prism::ProtoValue &proto_value)
            : unscaled_value(Utils::ProtoUtils::string_to_vector(proto_value.big_decimal().unscaled_value())),
              scale(proto_value.big_decimal().scale()) {
    }

    BigDecimal::BigDecimal(uint32_t scale, const std::vector <uint8_t> unscaled_value)
            : scale(scale), unscaled_value(unscaled_value) {}

    uint32_t BigDecimal::get_scale() const {
        return scale;
    }

    const std::vector <uint8_t> &BigDecimal::get_unscaled_value() const {
        return unscaled_value;
    }
} // Types