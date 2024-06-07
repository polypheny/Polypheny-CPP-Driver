//
// Created by tobi on 01.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_CONNECTIONCLOSEDERROR_H
#define POLYPHENY_CPP_DRIVER_CONNECTIONCLOSEDERROR_H

#include <exception>
#include <string>

namespace Errors {

    class ConnectionClosedError : public std::exception {
    public:
        explicit ConnectionClosedError(const std::string &message);

        virtual const char *what() const noexcept override;

    private:
        std::string message;

    };

} // Errors

#endif //POLYPHENY_CPP_DRIVER_CONNECTIONCLOSEDERROR_H
