#ifndef POLYPHENY_CPP_DRIVER_ROW_H
#define POLYPHENY_CPP_DRIVER_ROW_H

#include <vector>
#include <memory>
#include <string>
#include "relational_frame.pb.h"
#include "RelationalMetadata.h"
#include "types/TypedValue.h"

namespace Results {

    class Row {

    public:
        Row(const org::polypheny::prism::Row &row, std::shared_ptr<Results::RelationalMetadata> relational_metadata);

        Types::TypedValue &operator[](size_t column_index);

        const Types::TypedValue &operator[](size_t column_index) const;

        Types::TypedValue &operator[](const std::string &column_name);

        const Types::TypedValue &operator[](const std::string &column_name) const;

        Types::TypedValue at(size_t column_index);

        Types::TypedValue at(const std::string &column_name);

        [[nodiscard]] Types::TypedValue at(size_t column_index) const;

        [[nodiscard]] Types::TypedValue at(const std::string &column_name) const;

        [[nodiscard]] size_t size() const;

        using iterator = std::vector<Types::TypedValue>::iterator;
        using const_iterator = std::vector<Types::TypedValue>::const_iterator;

        iterator begin() { return values.begin(); }

        [[nodiscard]] const_iterator begin() const { return values.begin(); }

        iterator end() { return values.end(); }

        [[nodiscard]] const_iterator end() const { return values.end(); }

    private:
        std::vector<Types::TypedValue> values;
        std::shared_ptr<Results::RelationalMetadata> relational_metadata;
    };

} // namespace Results

#endif // POLYPHENY_CPP_DRIVER_ROW_H
