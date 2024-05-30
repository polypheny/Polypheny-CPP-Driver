#ifndef POLYPHENY_CPP_DRIVER_DOCUMENT_H
#define POLYPHENY_CPP_DRIVER_DOCUMENT_H


#include <unordered_map>
#include <string>
#include "TypedValue.h"
#include "ProtoValue.pb.h"
#include "ProtoUtils.h"

namespace Types {

    class Document : public std::unordered_map<std::string, TypedValue> {
    public:
        Document();

        Document(const ProtoDocument &document);

        ProtoDocument serialize() const;
    };

} // Types

#endif //POLYPHENY_CPP_DRIVER_DOCUMENT_H
