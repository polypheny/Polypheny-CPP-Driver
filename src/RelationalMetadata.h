//
// Created by tobi on 06.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_RELATIONALMETADATA_H
#define POLYPHENY_CPP_DRIVER_RELATIONALMETADATA_H

#include "google/protobuf/repeated_ptr_field.h"

namespace Results {

    class RelationalMetadata {

    public:
        RelationalMetadata(const google::protobuf::RepeatedPtrField<org::polypheny::prism::ColumnMeta> field);
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_RELATIONALMETADATA_H
