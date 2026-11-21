#include "DocumentResult.h"

#include <unordered_map>
#include <utility>
#include <vector>
#include <stdexcept>

#include "connection/Cursor.h"
#include "connection/Connection.h"
#include "types/TypedValue.h"
#include <limits>

namespace Results {

    DocumentResult::DocumentResult(const org::polypheny::prism::DocumentFrame &document_frame,
                                   bool is_last, Connection::Cursor *cursor)
            : Result(ResultType::DOCUMENT, cursor->get_connection().get_prism_interface_client(),
                     cursor->get_statement_id(), cursor->get_fetch_size()),
              is_fully_fetched(is_last) {
        add_documents(document_frame);
    }

    void DocumentResult::add_documents(const org::polypheny::prism::DocumentFrame &document_frame) {
        for (const auto &proto_document : document_frame.documents()) {
            std::unordered_map<std::string, Types::TypedValue> document;
            for (const auto &field : proto_document.entries()) {
                document[field.first] = Types::TypedValue(field.second, client);
            }
            documents.push_back(document);
        }
    }

    void DocumentResult::fetch_more() {
        const auto frame = fetch_frame();

        if (frame.result_case() != org::polypheny::prism::Frame::ResultCase::kDocumentFrame) {
            throw std::runtime_error("Statement returned a result of illegal type.");
        }

        is_fully_fetched = frame.is_last();
        add_documents(frame.document_frame());
    }

    DocumentResult::DocumentIterator DocumentResult::begin() {
        ensure_available(0);
        return {std::shared_ptr<DocumentResult>(this, [](DocumentResult*){}), 0};
    }

    DocumentResult::DocumentIterator DocumentResult::end() {
        return {std::shared_ptr<DocumentResult>(this, [](DocumentResult*){}), std::numeric_limits<size_t>::max()};
    }

    void DocumentResult::ensure_available(size_t index) {
        if (is_closed()) {
            throw std::runtime_error("The result is closed");
        }
        while (index >= documents.size() && !is_fully_fetched) {
            fetch_more();
        }
    }

    DocumentResult::DocumentIterator::DocumentIterator(std::shared_ptr<DocumentResult> result, size_t index)
            : result(std::move(result)), index(index) {}

    bool DocumentResult::DocumentIterator::operator==(const DocumentIterator &other) const {
        if (result != other.result) {
            return false;
        }
        const auto sentinel = std::numeric_limits<size_t>::max();
        if (index == sentinel || other.index == sentinel) {
            const auto position = index == sentinel ? other.index : index;
            return position == sentinel || (result->is_fully_fetched && position >= result->documents.size());
        }
        return index == other.index;
    }

    bool DocumentResult::DocumentIterator::operator!=(const DocumentIterator &other) const {
        return !(*this == other);
    }

    std::unordered_map<std::string, Types::TypedValue>& DocumentResult::DocumentIterator::operator*() {
        result->ensure_available(index);
        return result->documents.at(index);
    }

    DocumentResult::DocumentIterator &DocumentResult::DocumentIterator::operator++() {
        ++index;
        result->ensure_available(index);
        return *this;
    }

} // Results
