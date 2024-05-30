#include "BigDecimal.h"

namespace Types {
    BigDecimal::BigDecimal(uint32_t scale, const std::string unscaled_value)
            : scale(scale), unscaled_value(unscaled_value) {}

    uint32_t BigDecimal::get_scale() const {
        return scale;
    }

    const std::string &BigDecimal::get_unscaled_value() const {
        return unscaled_value;
    }
} // Types