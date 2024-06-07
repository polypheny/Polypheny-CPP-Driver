//
// Created by tobi on 06.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_ROW_H
#define POLYPHENY_CPP_DRIVER_ROW_H

#include "relational_frame.pb.h"
#include "RelationalMetadata.h"

namespace Results {

    class Row {

    public:
        Row(const org::polypheny::prism::Row row, std::shared_ptr<Results::RelationalMetadata> sharedPtr);
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_ROW_H
