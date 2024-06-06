#include "ServerError.h"

namespace Errors {
    ServerError::ServerError(const std::string &message) : message(message) {}

    const char *ServerError::what() const noexcept {
        return message.c_str();
    }
} // Errors