//
// Created by tobi on 30.05.24.
//

#include "ProtoUtils.h"

namespace Utils {
    std::vector<uint8_t> ProtoUtils::string_to_vector(const std::string &string) {
        return {string.begin(), string.end()};
    }

    std::string ProtoUtils::vector_to_string(const std::vector<uint8_t> &vector) {
        return {vector.begin(), vector.end()};
    }
}