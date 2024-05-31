#include "Document.h"

namespace Types {

    Document::Document() = default;

    Document::Document(const org::polypheny::prism::ProtoDocument &document) {
        for (const auto &entry: document.entries()) {
            if (entry.has_key() && entry.key().has_string()) {
                entries.emplace(entry.key().string().string(), TypedValue(entry.value()));
            }
        }
    }

    org::polypheny::prism::ProtoDocument Document::serialize() const {
        org::polypheny::prism::ProtoDocument proto_document;
        google::protobuf::RepeatedPtrField<org::polypheny::prism::ProtoEntry> *proto_entries = proto_document.mutable_entries();
        for (const auto &entry: entries) {
            org::polypheny::prism::ProtoEntry *proto_entry = proto_entries->Add();
            proto_entry->mutable_key()->mutable_string()->set_string(entry.first);
            *(proto_entry->mutable_value()) = entry.second.serialize();
        }
        return proto_document;
    }

    TypedValue &Document::operator[](const std::string &key) {
        return entries[key];
    }

    const TypedValue &Document::at(const std::string &key) const {
        return entries.at(key);
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

    size_t Document::erase(const std::string &key) {
        return entries.erase(key);
    }

    size_t Document::count(const std::string &key) const {
        return entries.count(key);
    }

    std::unordered_map<std::string, TypedValue>::iterator Document::find(const std::string &key) {
        return entries.find(key);
    }

    std::unordered_map<std::string, TypedValue>::const_iterator Document::find(const std::string &key) const {
        return entries.find(key);
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

    std::pair<std::unordered_map<std::string, TypedValue>::iterator, bool>
    Document::insert(const std::pair<std::string, TypedValue> &value) {
        return entries.insert(value);
    }

} // namespace Types
