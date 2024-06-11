//
// Created by tobi on 06.06.24.
//

#include "Row.h"

#include <utility>

namespace Results {

    Row::Row(const org::polypheny::prism::Row &row, std::shared_ptr<Results::RelationalMetadata> relational_metadata)
            : relational_metadata(std::move(relational_metadata)) {
        for (const auto& value : row.values()) {
            values.emplace_back(value);
        }

    }

    Types::TypedValue &Row::operator[](size_t column_index) {
        return values[column_index];
    }

    const Types::TypedValue &Row::operator[](size_t column_index) const {
        return values[column_index];
    }

    Types::TypedValue &Row::operator[](const std::string &column_name) {
        return values[relational_metadata->get_column_index_from_label(column_name)];
    }

    const Types::TypedValue &Row::operator[](const std::string &column_name) const {
        return values[relational_metadata->get_column_index_from_label(column_name)];
    }

    Types::TypedValue Row::at(size_t column_index) {
        return values.at(column_index);
    }

    Types::TypedValue Row::at(const std::string &column_name) {
        return values.at(relational_metadata->get_column_index_from_label(column_name));
    }

    Types::TypedValue Row::at(size_t column_index) const {
        return values.at(column_index);
    }

    Types::TypedValue Row::at(const std::string &column_name) const {
        return values.at(relational_metadata->get_column_index_from_label(column_name));
    }

    size_t Row::size() const {
        return values.size();
    }




} // Results