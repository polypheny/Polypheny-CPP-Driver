#ifndef POLYPHENY_CPP_DRIVER_BIGDECIMAL_H
#define POLYPHENY_CPP_DRIVER_BIGDECIMAL_H

#include "NativeType.h"
#include <cstdint>
#include <vector>
#include <string>
#include "value.pb.h"
#include "ProtoUtils.h"
#include "TypedValue.h"

namespace Types {
    // forward declarations
    class TypedValue;

    class BigDecimal : public NativeType {
    private:
        uint32_t scale;
        std::vector<uint8_t> unscaled_value;

    public:
        BigDecimal(const org::polypheny::prism::ProtoBigDecimal &value);

        BigDecimal(uint32_t scale, std::vector<uint8_t> unscaled_value);

        uint32_t get_scale() const;

        const std::vector<uint8_t> &get_unscaled_value() const;

        std::unique_ptr<org::polypheny::prism::ProtoBigDecimal> serialize() const;
    };

} // namespace Types

#endif // POLYPHENY_CPP_DRIVER_BIGDECIMAL_H
