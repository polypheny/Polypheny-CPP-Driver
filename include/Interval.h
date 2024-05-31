//
// Created by tobi on 30.05.24.
//

#ifndef POLYPHENY_CPP_DRIVER_INTERVAL_H
#define POLYPHENY_CPP_DRIVER_INTERVAL_H

#include "NativeType.h"
#include "value.pb.h"
#include <string>

namespace Types {
    class Interval : public NativeType  {
    private:
        uint64_t months;
        uint64_t milliseconds;

        static std::string plural(uint64_t count, const std::string &word);

    public:
        Interval(const org::polypheny::prism::ProtoInterval& value);
        Interval(uint64_t months, uint64_t milliseconds);

        uint64_t get_months() const;

        uint64_t get_milliseconds() const;

        bool equals(const Interval &other) const;

        org::polypheny::prism::ProtoInterval serialize() const;

        friend std::ostream & operator<<(std::ostream& os, const Interval& interval);
    };

} // Types

#endif //POLYPHENY_CPP_DRIVER_INTERVAL_H
