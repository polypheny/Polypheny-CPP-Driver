//
// Created by tobi on 06.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_RELATIONALCOLUMNMETADATA_H
#define POLYPHENY_CPP_DRIVER_RELATIONALCOLUMNMETADATA_H

#include <string>
#include "relational_frame.pb.h"

namespace Results {

    /**
     * @brief Represents metadata for a column in a relational query result.
     *
     * This class provides access to various properties of a column, such as its index,
     * whether it is nullable, its length, label, name, precision, type name, and scale.
     */
    class RelationalColumnMetadata {
    public:
        explicit RelationalColumnMetadata(const org::polypheny::prism::ColumnMeta &columnMeta);

        /**
         * @brief Gets the zero based index of the column.
         *
         * @return The column index.
         */
        [[nodiscard]] uint32_t get_column_index() const;

        /**
         * @brief Gets whether the column is nullable.
         *
         * @return True if the column is nullable, false otherwise.
         */
        [[nodiscard]] bool get_is_nullable() const;

        /**
         * @brief Gets the length of the column.
         *
         * @return The column length.
         */
        [[nodiscard]] uint32_t get_length() const;

        /**
         * @brief Gets the label of the column.
         *
         * @return The column label.
         */
        [[nodiscard]] std::string get_column_label() const;

        /**
        * @brief Gets the name of the column.
        *
        * @return The column name.
        */
        [[nodiscard]] std::string get_column_name() const;

        /**
         * @brief Gets the precision of the column.
         *
         * @return The column precision.
         */
        [[nodiscard]] uint32_t get_precision() const;

        /**
         * @brief Gets the protocol type name of the column.
         *
         * @return The protocol type name.
         */
        [[nodiscard]] std::string get_protocol_type_name() const;

        /**
         * @brief Gets the scale of the column.
         *
         * @return The column scale.
         */
        [[nodiscard]] uint32_t get_scale() const;

    private:
        uint32_t columnIndex;
        bool isNullable;
        uint32_t length;
        std::string columnLabel;
        std::string columnName;
        uint32_t precision;
        std::string protocolTypeName;
        uint32_t scale;

        static std::string get_protocol_type_name(org::polypheny::prism::ProtoPolyType type);
    };
}


#endif //POLYPHENY_CPP_DRIVER_RELATIONALCOLUMNMETADATA_H
