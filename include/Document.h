#ifndef POLYPHENY_CPP_DRIVER_DOCUMENT_H
#define POLYPHENY_CPP_DRIVER_DOCUMENT_H

#include <unordered_map>
#include <string>
#include "TypedValue.h"
#include "protointerface.pb.h"
#include "ProtoUtils.h"
#include "value.pb.h"
#include "NativeType.h"

namespace Types {
    // forward declarations
    class TypedValue;
}

namespace Types {

    class Document : public NativeType {
    public:
        Document() = default;

        explicit Document(const org::polypheny::prism::ProtoDocument &document);

        std::unique_ptr<org::polypheny::prism::ProtoDocument> serialize() const;

    private:
    };

} // namespace Types

#endif // POLYPHENY_CPP_DRIVER_DOCUMENT_H
