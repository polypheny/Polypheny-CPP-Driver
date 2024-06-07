//
// Created by tobi on 01.06.24.
//

#include "ConnectionClosedError.h"

namespace Errors {
    ConnectionClosedError::ConnectionClosedError(const std::string &message) : message(message) {}

    const char *ConnectionClosedError::what() const noexcept {
        return message.c_str();
    }
} // Errors