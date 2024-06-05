#include "Document.h"
#include "value.pb.h"

namespace Types {
    Document::Document(const org::polypheny::prism::ProtoDocument &document) {
        for (const auto &entry: document.entries()) {
            if (entry.has_key() && entry.key().has_string()) {
                entries.emplace(entry.key().string().string(), TypedValue(entry.value()));
            }
        }
    }

    std::unique_ptr<org::polypheny::prism::ProtoDocument> Document::serialize() {
        auto proto_document = std::make_unique<org::polypheny::prism::ProtoDocument>();
        google::protobuf::RepeatedPtrField<org::polypheny::prism::ProtoEntry> *proto_entries = proto_document->mutable_entries();
        for (auto &entry: entries) {
            org::polypheny::prism::ProtoEntry *proto_entry = proto_entries->Add();
            proto_entry->mutable_key()->mutable_string()->set_string(entry.first);
            *(proto_entry->mutable_value()) = *entry.second.serialize();  // Assuming serialize returns a unique_ptr
        }
        return proto_document;
    }

    TypedValue& Document::operator[](const std::string &key) {
        return entries[key];
    }

    std::pair<std::unordered_map<std::string, TypedValue>::iterator, bool> Document::insert(const std::pair<std::string, TypedValue> &pair) {
        return entries.insert(pair);
    }

    size_t Document::erase(const std::string &key) {
        return entries.erase(key);
    }

    std::unordered_map<std::string, TypedValue>::iterator Document::find(const std::string &key) {
        return entries.find(key);
    }

    std::unordered_map<std::string, TypedValue>::const_iterator Document::find(const std::string &key) const {
        return entries.find(key);
    }

    size_t Document::size() const {
        return entries.size();
    }

    bool Document::empty() const {
        return entries.empty();
    }

    void Document::clear() {
        entries.clear();
    }

    std::unordered_map<std::string, TypedValue>::iterator Document::begin() {
        return entries.begin();
    }

    std::unordered_map<std::string, TypedValue>::const_iterator Document::begin() const {
        return entries.begin();
    }

    std::unordered_map<std::string, TypedValue>::iterator Document::end() {
        return entries.end();
    }

    std::unordered_map<std::string, TypedValue>::const_iterator Document::end() const {
        return entries.end();
    }

} // namespace Types
