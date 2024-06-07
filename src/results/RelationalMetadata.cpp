//
// Created by tobi on 06.06.24.
//

#include "RelationalMetadata.h"

namespace Results {
    RelationalMetadata::RelationalMetadata(org::polypheny::prism::RelationalFrame relational_frame) {
        for (const auto& meta : relational_frame.column_meta()) {
            column_metas.emplace_back(meta);
        }

        for (uint32_t i = 0; i < relational_frame.column_meta_size(); ++i) {
            column_indexes[column_metas[i].get_column_label()] = i;
        }
    }

    uint32_t RelationalMetadata::get_column_index_from_label(const std::string &column_label) const {
        return column_indexes.at(column_label);
    }

    uint32_t RelationalMetadata::get_column_count() const {
        return column_metas.size();
    }

    RelationalColumnMetadata RelationalMetadata::get_column_meta(uint32_t column_index) const {
        return column_metas[column_index];
    }
} // Results