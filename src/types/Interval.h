//
// Created by tobi on 30.05.24.
//

#ifndef POLYPHENY_CPP_DRIVER_INTERVAL_H
#define POLYPHENY_CPP_DRIVER_INTERVAL_H

#include "NativeType.h"
#include "value.pb.h"
#include <string>

namespace Types {
    /**
     * @brief Represents the Polypheny internal interval type denoting a time interval with months and milliseconds.
     *
     */
    class Interval : public NativeType  {
    private:
        uint64_t months;
        uint64_t milliseconds;

        static std::string plural(uint64_t count, const std::string &word);

    public:
        Interval(const org::polypheny::prism::ProtoInterval& value);

        /**
         * @brief Constructs an Interval object with the specified number of months and milliseconds.
         *
         * @param months The number of months in the interval.
         * @param milliseconds The number of milliseconds in the interval.
         */
        Interval(uint64_t months, uint64_t milliseconds);

        /**
         * @brief Gets the number of months in the interval.
         *
         * @return The number of months.
         */
        uint64_t get_months() const;

        /**
        * @brief Gets the number of milliseconds in the interval.
        *
        * @return The number of milliseconds.
        */
        uint64_t get_milliseconds() const;

        /**
         * @brief Checks if this interval is equal to another interval.
         *
         * @param other The other interval to compare with.
         * @return True if the intervals are equal, false otherwise.
         */
        bool equals(const Interval &other) const;

        std::unique_ptr<org::polypheny::prism::ProtoInterval> serialize() const;

        /**
         * @brief Stream insertion operator for printing the interval.
         *
         * @param os The output stream.
         * @param interval The interval to print.
         * @return A reference to the output stream.
         */
        friend std::ostream & operator<<(std::ostream& os, const Interval& interval);
    };

} // Types

#endif //POLYPHENY_CPP_DRIVER_INTERVAL_H
