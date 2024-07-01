#ifndef POLYPHENY_CPP_DRIVER_ROW_H
#define POLYPHENY_CPP_DRIVER_ROW_H

#include <vector>
#include <memory>
#include <string>
#include "relational_frame.pb.h"
#include "RelationalMetadata.h"
#include "types/TypedValue.h"

namespace Results {

    /**
     * @brief Represents a single row in the result of a relational query.
     *
     * This class provides access to the values in a row, allowing access by column index or column name.
     * It also supports iteration over the values in the row.
     */
    class Row {

    public:
        Row(const org::polypheny::prism::Row &row, std::shared_ptr<Results::RelationalMetadata> relational_metadata);

        /**
         * @brief Gets the value at the specified zero based column index.
         *
         * @param column_index The zero based index of the column.
         * @return A reference to the value at the specified column index.
         */
        Types::TypedValue &operator[](size_t column_index);

        /**
         * @brief Gets the value at the specified zero based column index (const version).
         *
         * @param column_index The index of the column.
         * @return A const reference to the value at the specified column index.
         */
        const Types::TypedValue &operator[](size_t column_index) const;

        /**
         * @brief Gets the value at the specified column name.
         *
         * @param column_name The name of the column.
         * @return A reference to the value at the specified column name.
         */
        Types::TypedValue &operator[](const std::string &column_name);

        /**
         * @brief Gets the value at the specified column name (const version).
         *
         * @param column_name The name of the column.
         * @return A const reference to the value at the specified column name.
         */
        const Types::TypedValue &operator[](const std::string &column_name) const;

        /**
         * @brief Gets the value at the specified zero based column index.
         *
         * @param column_index The index of the column.
         * @return The value at the specified column index.
         */
        Types::TypedValue at(size_t column_index);

        /**
         * @brief Gets the value at the specified column name.
         *
         * @param column_name The name of the column.
         * @return The value at the specified column name.
         */
        Types::TypedValue at(const std::string &column_name);

        /**
         * @brief Gets the value at the specified zero based column index (const version).
         *
         * @param column_index The index of the column.
         * @return The value at the specified column index.
         */
        [[nodiscard]] Types::TypedValue at(size_t column_index) const;

        /**
         * @brief Gets the value at the specified column name (const version).
         *
         * @param column_name The name of the column.
         * @return The value at the specified column name.
         */
        [[nodiscard]] Types::TypedValue at(const std::string &column_name) const;

        /**
         * @brief Gets the number of columns in the row.
         *
         * @return The number of columns in the row.
         */
        [[nodiscard]] size_t size() const;

        using iterator = std::vector<Types::TypedValue>::iterator;
        using const_iterator = std::vector<Types::TypedValue>::const_iterator;

        /**
         * @brief Returns an iterator to the beginning of the row's values.
         *
         * @return An iterator to the first value.
         */
        iterator begin() { return values.begin(); }

        /**
         * @brief Returns a const iterator to the beginning of the row's values.
         *
         * @return A const iterator to the first value.
         */
        [[nodiscard]] const_iterator begin() const { return values.begin(); }

        /**
         * @brief Returns an iterator to the end of the row's values.
         *
         * @return An iterator to the past-the-end value.
         */
        iterator end() { return values.end(); }

        /**
         * @brief Returns a const iterator to the end of the row's values.
         *
         * @return A const iterator to the past-the-end value.
         */
        [[nodiscard]] const_iterator end() const { return values.end(); }

    private:
        std::vector<Types::TypedValue> values;
        std::shared_ptr<Results::RelationalMetadata> relational_metadata;
    };

} // namespace Results

#endif // POLYPHENY_CPP_DRIVER_ROW_H
