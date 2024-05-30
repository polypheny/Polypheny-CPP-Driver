//
// Created by tobi on 30.05.24.
//

#include "ProtoUtils.h"

namespace Utils {
    std::vector<uint8_t> ProtoUtils::string_to_vector(const std::string &string) {
        return std::vector<uint8_t>(input.begin(), input.end());
    }

    std::string ProtoUtils::vector_to_string(const std::vector<uint8_t> &vector) {
        return std::string(input.begin(), input.end());
    }
}