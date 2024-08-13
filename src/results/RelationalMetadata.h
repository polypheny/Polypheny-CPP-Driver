//
// Created by tobi on 06.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_RELATIONALMETADATA_H
#define POLYPHENY_CPP_DRIVER_RELATIONALMETADATA_H

#include "RelationalColumnMetadata.h"

#include "org/polypheny/prism/relational_frame.pb.h"

namespace Results {

    /**
     * @brief Represents metadata for the columns of a relational query result.
     *
     * This class provides access to column metadata, including methods to retrieve
     * column metadata by index, get the index of a column by its label, and get the total column count.
     */
    class RelationalMetadata {

    public:
        explicit RelationalMetadata(org::polypheny::prism::RelationalFrame relational_frame);

        /**
        * @brief Gets the metadata for a specific column by its index.
        *
        * @param column_index The index of the column.
        * @return The metadata of the specified column.
        */
        RelationalColumnMetadata get_column_meta(uint32_t column_index) const;

/**
         * @brief Gets the index of a column by its label.
         *
         * @param column_label The label of the column.
         * @return The index of the specified column.
         */
        uint32_t get_column_index_from_label(const std::string &column_label) const;

        /**
         * @brief Constructs a RelationalMetadata object from a RelationalFrame protobuf.
         *
         * @param relational_frame The RelationalFrame protobuf containing relational metadata.
         */
        uint32_t get_column_count() const;

    private:
        std::vector<RelationalColumnMetadata> column_metas;
        std::unordered_map<std::string, uint32_t> column_indexes;

    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_RELATIONALMETADATA_H
