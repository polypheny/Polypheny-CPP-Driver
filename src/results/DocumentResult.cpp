#include "DocumentResult.h"
#include "connection/Cursor.h"
#include "connection/Connection.h"
#include "types/TypedValue.h"
#include <unordered_map>
#include <utility>
#include <vector>
#include <stdexcept>

namespace Results {

    DocumentResult::DocumentResult(const org::polypheny::prism::DocumentFrame &document_frame,
                                   bool is_last, Connection::Cursor *cursor)
            : Result(ResultType::DOCUMENT), is_fully_fetched(is_last), cursor(cursor) {
        add_documents(document_frame);
    }

    void DocumentResult::add_documents(const org::polypheny::prism::DocumentFrame &document_frame) {
        for (const auto &proto_document : document_frame.documents()) {
            std::unordered_map<std::string, Types::TypedValue> document;
            for (const auto &field : proto_document.entries()) {
                document[field.key().string().string()] = Types::TypedValue(field.value());
            }
            documents.push_back(document);
        }
    }

    void DocumentResult::fetch_more() {
        uint32_t statement_id = cursor->get_statement_id();
        org::polypheny::prism::Frame frame = cursor->get_connection().get_prism_interface_client().fetch_result(
                statement_id, DEFAULT_FETCH_SIZE);

        if (frame.result_case() != org::polypheny::prism::Frame::ResultCase::kDocumentFrame) {
            throw std::runtime_error("Statement returned a result of illegal type.");
        }

        is_fully_fetched = frame.is_last();
        add_documents(frame.document_frame());
    }

    DocumentResult::DocumentIterator DocumentResult::begin() {
        return {std::shared_ptr<DocumentResult>(this, [](DocumentResult*){}), 0};
    }

    DocumentResult::DocumentIterator DocumentResult::end() {
        return {std::shared_ptr<DocumentResult>(this, [](DocumentResult*){}), documents.size()};
    }

    DocumentResult::DocumentIterator::DocumentIterator(std::shared_ptr<DocumentResult> result, size_t index)
            : result(std::move(result)), index(index) {}

    bool DocumentResult::DocumentIterator::operator==(const DocumentIterator &other) const {
        return index == other.index && result == other.result;
    }

    bool DocumentResult::DocumentIterator::operator!=(const DocumentIterator &other) const {
        return !(*this == other);
    }

    std::unordered_map<std::string, Types::TypedValue>& DocumentResult::DocumentIterator::operator*() {
        return *std::next(result->documents.begin(), index);
    }

    DocumentResult::DocumentIterator &DocumentResult::DocumentIterator::operator++() {
        ++index;
        if (index >= result->documents.size() && !result->is_fully_fetched) {
            result->fetch_more();
        }
        return *this;
    }

} // Results
