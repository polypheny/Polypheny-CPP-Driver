//
// Created by tobi on 06.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_SERVERERROR_H
#define POLYPHENY_CPP_DRIVER_SERVERERROR_H

#include <exception>
#include <string>

namespace Errors {

    class ServerError : public std::exception {
    public:
        explicit ServerError(const std::string &message);

        virtual const char *what() const noexcept override;

    private:
        std::string message;

    };

} // Errors

#endif //POLYPHENY_CPP_DRIVER_SERVERERROR_H
