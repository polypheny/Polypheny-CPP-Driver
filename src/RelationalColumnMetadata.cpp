#include "RelationalColumnMetadata.h"

namespace Results {
    RelationalColumnMetadata::RelationalColumnMetadata(const org::polypheny::prism::ColumnMeta &columnMeta)
            : columnIndex(columnMeta.column_index()),
              isNullable(columnMeta.is_nullable()),
              length(columnMeta.length()),
              columnLabel(columnMeta.column_label()),
              columnName(columnMeta.column_name()),
              precision(columnMeta.precision()),
              protocolTypeName(get_protocol_type_name(columnMeta.type_meta().proto_value_type())),
              scale(columnMeta.scale()) {}

    uint32_t RelationalColumnMetadata::get_column_index() const {
        return columnIndex;
    }

    bool RelationalColumnMetadata::get_is_nullable() const {
        return isNullable;
    }

    uint32_t RelationalColumnMetadata::get_length() const {
        return length;
    }

    std::string RelationalColumnMetadata::get_column_label() const {
        return columnLabel;
    }

    std::string RelationalColumnMetadata::get_column_name() const {
        return columnName;
    }

    uint32_t RelationalColumnMetadata::get_precision() const {
        return precision;
    }

    std::string RelationalColumnMetadata::get_protocol_type_name() const {
        return protocolTypeName;
    }

    uint32_t RelationalColumnMetadata::get_scale() const {
        return scale;
    }

    std::string RelationalColumnMetadata::get_protocol_type_name(org::polypheny::prism::ProtoPolyType type) {
        switch(type) {
            case org::polypheny::prism::ProtoPolyType::UNSPECIFIED:
                return "UNSPECIFIED";
            case org::polypheny::prism::ProtoPolyType::BOOLEAN:
                return "BOOLEAN";
            case org::polypheny::prism::ProtoPolyType::TINYINT:
                return "TINYINT";
            case org::polypheny::prism::ProtoPolyType::SMALLINT:
                return "SMALLINT";
            case org::polypheny::prism::ProtoPolyType::INTEGER:
                return "INTEGER";
            case org::polypheny::prism::ProtoPolyType::BIGINT:
                return "BIGINT";
            case org::polypheny::prism::ProtoPolyType::DECIMAL:
                return "DECIMAL";
            case org::polypheny::prism::ProtoPolyType::REAL:
                return "REAL";
            case org::polypheny::prism::ProtoPolyType::FLOAT:
                return "FLOAT";
            case org::polypheny::prism::ProtoPolyType::DOUBLE:
                return "DOUBLE";
            case org::polypheny::prism::ProtoPolyType::DATE:
                return "DATE";
            case org::polypheny::prism::ProtoPolyType::TIME:
                return "TIME";
            case org::polypheny::prism::ProtoPolyType::TIMESTAMP:
                return "TIMESTAMP";
            case org::polypheny::prism::ProtoPolyType::INTERVAL:
                return "INTERVAL";
            case org::polypheny::prism::ProtoPolyType::CHAR:
                return "CHAR";
            case org::polypheny::prism::ProtoPolyType::VARCHAR:
                return "VARCHAR";
            case org::polypheny::prism::ProtoPolyType::TEXT:
                return "TEXT";
            case org::polypheny::prism::ProtoPolyType::BINARY:
                return "BINARY";
            case org::polypheny::prism::ProtoPolyType::VARBINARY:
                return "VARBINARY";
            case org::polypheny::prism::ProtoPolyType::NULL_:
                return "NULL";
            case org::polypheny::prism::ProtoPolyType::ARRAY:
                return "ARRAY";
            case org::polypheny::prism::ProtoPolyType::MAP:
                return "MAP";
            case org::polypheny::prism::ProtoPolyType::DOCUMENT:
                return "DOCUMENT";
            case org::polypheny::prism::ProtoPolyType::GRAPH:
                return "GRAPH";
            case org::polypheny::prism::ProtoPolyType::NODE:
                return "NODE";
            case org::polypheny::prism::ProtoPolyType::EDGE:
                return "EDGE";
            case org::polypheny::prism::ProtoPolyType::PATH:
                return "PATH";
            case org::polypheny::prism::ProtoPolyType::IMAGE:
                return "IMAGE";
            case org::polypheny::prism::ProtoPolyType::VIDEO:
                return "VIDEO";
            case org::polypheny::prism::ProtoPolyType::AUDIO:
                return "AUDIO";
            case org::polypheny::prism::ProtoPolyType::FILE:
                return "FILE";
            case org::polypheny::prism::ProtoPolyType::DISTINCT:
                return "DISTINCT";
            case org::polypheny::prism::ProtoPolyType::STRUCTURED:
                return "STRUCTURED";
            case org::polypheny::prism::ProtoPolyType::ROW:
                return "ROW";
            case org::polypheny::prism::ProtoPolyType::OTHER:
                return "OTHER";
            case org::polypheny::prism::ProtoPolyType::CURSOR:
                return "CURSOR";
            case org::polypheny::prism::ProtoPolyType::COLUMN_LIST:
                return "COLUMN_LIST";
            case org::polypheny::prism::ProtoPolyType::DYNAMIC_STAR:
                return "DYNAMIC_STAR";
            case org::polypheny::prism::ProtoPolyType::GEOMETRY:
                return "GEOMETRY";
            case org::polypheny::prism::ProtoPolyType::SYMBOL:
                return "SYMBOL";
            case org::polypheny::prism::ProtoPolyType::JSON:
                return "JSON";
            case org::polypheny::prism::ProtoPolyType::MULTISET:
                return "MULTISET";
            case org::polypheny::prism::ProtoPolyType::ANY:
                return "ANY";
            case org::polypheny::prism::ProtoPolyType::USER_DEFINED_TYPE:
                return "USER_DEFINED_TYPE";
            case org::polypheny::prism::ProtoPolyType::ROW_ID:
                return "ROW_ID";
            default:
                return "UNKNOWN";
        }
    }

}
