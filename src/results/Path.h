//
// Created by tobi on 18.07.24.
//

#ifndef POLYPHENY_CPP_DRIVER_PATH_H
#define POLYPHENY_CPP_DRIVER_PATH_H

#include "results/GraphElement.h"
#include "graph_frame.pb.h"

namespace Results {

    class Path : public GraphElement{
    public:
        Path(const org::polypheny::prism::ProtoPath &proto_path);
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_PATH_H
