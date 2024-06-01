#include "Document.h"

namespace Types {
    Document::Document(const org::polypheny::prism::ProtoDocument &document) {
        for (const auto &entry: document.entries()) {
            if (entry.has_key() && entry.key().has_string()) {
                //entries.emplace(entry.key().string().string(), TypedValue(entry.value()));
            }
        }
    }


    std::unique_ptr<org::polypheny::prism::ProtoDocument> Document::serialize() const {
        auto proto_document = std::make_unique<org::polypheny::prism::ProtoDocument>();
        google::protobuf::RepeatedPtrField<org::polypheny::prism::ProtoEntry> *proto_entries = proto_document->mutable_entries();
        //for (const auto &entry: entries) {
        //    org::polypheny::prism::ProtoEntry *proto_entry = proto_entries->Add();
        //    proto_entry->mutable_key()->mutable_string()->set_string(entry.first);
        //    *(proto_entry->mutable_value()) = *entry.second.serialize();  // Assuming serialize returns a unique_ptr
        //}
        return proto_document;
    }

} // namespace Types
