//
// Created by tobi on 06.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_RELATIONALMETADATA_H
#define POLYPHENY_CPP_DRIVER_RELATIONALMETADATA_H

#include "relational_frame.pb.h"
#include "RelationalColumnMetadata.h"

namespace Results {

    class RelationalMetadata {

    public:
        explicit RelationalMetadata(org::polypheny::prism::RelationalFrame relational_frame);

        RelationalColumnMetadata get_column_meta(uint32_t column_index) const;

        uint32_t get_column_index_from_label(const std::string &column_label) const;

        uint32_t get_column_count() const;

    private:
        std::vector<RelationalColumnMetadata> column_metas;
        std::unordered_map<std::string, uint32_t> column_indexes;

    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_RELATIONALMETADATA_H
