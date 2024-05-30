//
// Created by tobi on 30.05.24.
//

#include "Interval.h"

namespace Types {
    Interval::Interval(uint64_t months, uint64_t milliseconds)
            : months(months), milliseconds(milliseconds) {}

    uint64_t Interval::get_months() const {
        return months;
    }

    uint64_t Interval::get_milliseconds() const {
        return milliseconds;
    }

    std::string Interval::plural(long count, const std::string &word) const {
        return std::to_string(count) + " " + (count != 1 ? word + "s" : word);
    }

    bool Interval::equals(const Interval &other) const {
        return months == other.get_months() && milliseconds == other.get_milliseconds();
    }

    std::ostream &operator<<(std::ostream &os, const Interval &interval) {
        os << plural(interval.get_months(), "month") + " " + plural(interval.get_milliseconds(), "millisecond");
        return os;
    }
} // Types