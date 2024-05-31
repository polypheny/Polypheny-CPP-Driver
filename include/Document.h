#ifndef POLYPHENY_CPP_DRIVER_DOCUMENT_H
#define POLYPHENY_CPP_DRIVER_DOCUMENT_H

#include <unordered_map>
#include <string>
#include "TypedValue.h"
#include "protointerface.pb.h"
#include "ProtoUtils.h"
#include "value.pb.h"

namespace Types {

    class Document {
    public:
        Document();

        explicit Document(const org::polypheny::prism::ProtoDocument &document);

        org::polypheny::prism::ProtoDocument serialize() const;

        TypedValue &operator[](const std::string &key);

        const TypedValue &at(const std::string &key) const;

        size_t size() const;

        bool empty() const;

        void clear();

        size_t erase(const std::string &key);

        size_t count(const std::string &key) const;

        std::unordered_map<std::string, TypedValue>::iterator find(const std::string &key);

        std::unordered_map<std::string, TypedValue>::const_iterator find(const std::string &key) const;

        std::unordered_map<std::string, TypedValue>::iterator begin();

        std::unordered_map<std::string, TypedValue>::const_iterator begin() const;

        std::unordered_map<std::string, TypedValue>::iterator end();

        std::unordered_map<std::string, TypedValue>::const_iterator end() const;

        std::pair<std::unordered_map<std::string, TypedValue>::iterator, bool>
        insert(const std::pair<std::string, TypedValue> &value);

    private:
        std::unordered_map<std::string, TypedValue> entries;
    };

} // namespace Types

#endif // POLYPHENY_CPP_DRIVER_DOCUMENT_H
