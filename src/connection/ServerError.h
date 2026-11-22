//
// Created by tobi on 06.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_SERVERERROR_H
#define POLYPHENY_CPP_DRIVER_SERVERERROR_H

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>

namespace Errors {

    class ServerError : public std::runtime_error {
    public:
        explicit ServerError(const std::string &message);

        ServerError(const std::string &message, std::optional<std::string> sql_state,
                    std::optional<std::int32_t> error_code);

        const char *what() const noexcept override;

        [[nodiscard]] const std::optional<std::string> &get_sql_state() const noexcept;

        [[nodiscard]] std::optional<std::int32_t> get_error_code() const noexcept;

    private:
        std::optional<std::string> sql_state;
        std::optional<std::int32_t> error_code;
    };

} // Errors

#endif //POLYPHENY_CPP_DRIVER_SERVERERROR_H
