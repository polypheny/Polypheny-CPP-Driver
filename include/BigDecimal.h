//
// Created by tobi on 30.05.24.
//

#ifndef POLYPHENY_CPP_DRIVER_BIGDECIMAL_H
#define POLYPHENY_CPP_DRIVER_BIGDECIMAL_H

#include "NativeType.h"
#include <cstdint>
#include <vector>
#include <string>

namespace Types {
    class BigDecimal : public NativeType {
    private:
        uint32_t scale;
        std::vector<uint8_t> unscaled_value;

    public:
        BigDecimal(uint32_t scale, std::vector<uint8_t> unscaled_value);

        uint32_t get_scale() const;

        const std::vector<uint8_t> &get_unscaled_value() const;
    };

} // Types

#endif //POLYPHENY_CPP_DRIVER_BIGDECIMAL_H
