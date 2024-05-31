#ifndef POLYPHENY_CPP_DRIVER_DOCUMENT_H
#define POLYPHENY_CPP_DRIVER_DOCUMENT_H


#include <unordered_map>
#include <string>
#include "TypedValue.h"
#include "protointerface.pb.h"
#include "ProtoUtils.h"

namespace Types {

    class Document {
    public:
        Document();

        explicit Document(const ProtoDocument &document);

        ProtoDocument serialize() const;
    };

} // Types

#endif //POLYPHENY_CPP_DRIVER_DOCUMENT_H
