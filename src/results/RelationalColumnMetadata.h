//
// Created by tobi on 06.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_RELATIONALCOLUMNMETADATA_H
#define POLYPHENY_CPP_DRIVER_RELATIONALCOLUMNMETADATA_H

#include <string>
#include "relational_frame.pb.h"

namespace Results {
    class RelationalColumnMetadata {
    public:
        // Constructor
        explicit RelationalColumnMetadata(const org::polypheny::prism::ColumnMeta &columnMeta);

        // Accessors
        [[nodiscard]] uint32_t get_column_index() const;

        [[nodiscard]] bool get_is_nullable() const;

        [[nodiscard]] uint32_t get_length() const;

        [[nodiscard]] std::string get_column_label() const;

        [[nodiscard]] std::string get_column_name() const;

        [[nodiscard]] uint32_t get_precision() const;

        [[nodiscard]] std::string get_protocol_type_name() const;

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
