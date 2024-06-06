//
// Created by tobi on 06.06.24.
//

#include "TransportUtils.h"

namespace Utils::TransportUtils {
    uint64_t reverse_byte_order(uint64_t value) {
        return ((value & 0xFF00000000000000) >> 56) |
               ((value & 0x00FF000000000000) >> 40) |
               ((value & 0x0000FF0000000000) >> 24) |
               ((value & 0x000000FF00000000) >> 8) |
               ((value & 0x00000000FF000000) << 8) |
               ((value & 0x0000000000FF0000) << 24) |
               ((value & 0x000000000000FF00) << 40) |
               ((value & 0x00000000000000FF) << 56);
    }
}

