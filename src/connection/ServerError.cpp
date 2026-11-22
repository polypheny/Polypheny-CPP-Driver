#include "ServerError.h"

#include <utility>

namespace Errors {
    ServerError::ServerError(const std::string &message)
            : ServerError(message, std::nullopt, std::nullopt) {}

    ServerError::ServerError(const std::string &message, std::optional<std::string> sql_state,
                             std::optional<std::int32_t> error_code)
            : std::runtime_error(message), sql_state(std::move(sql_state)), error_code(error_code) {}

    const char *ServerError::what() const noexcept {
        return std::runtime_error::what();
    }

    const std::optional<std::string> &ServerError::get_sql_state() const noexcept {
        return sql_state;
    }

    std::optional<std::int32_t> ServerError::get_error_code() const noexcept {
        return error_code;
    }
} // Errors
