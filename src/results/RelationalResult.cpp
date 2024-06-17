#include "RelationalResult.h"
#include "src/connection/Cursor.h"
#include "src/connection/Connection.h"

namespace Results {
    RelationalResult::RelationalResult(const org::polypheny::prism::RelationalFrame &relational_frame,
                                       bool is_last, Connection::Cursor *cursor)
            : Result(ResultType::RELATIONAL), is_fully_fetched(is_last), cursor(cursor) {
        metadata = std::make_shared<RelationalMetadata>(relational_frame);
        add_rows(relational_frame);
    }

    RelationalResult::~RelationalResult() {
        uint32_t statement_id = cursor->get_statement_id();
        cursor->get_connection().get_prism_interface_client().close_result(statement_id);
    }

    void RelationalResult::add_rows(const org::polypheny::prism::RelationalFrame &relationalFrame) {
        for (const auto &row : relationalFrame.rows()) {
            rows.emplace_back(row, metadata);
        }
    }

    void RelationalResult::fetch_more() {
        uint32_t statement_id = cursor->get_statement_id();
        org::polypheny::prism::Frame frame = cursor->get_connection().get_prism_interface_client().fetch_result(
                statement_id, DEFAULT_FETCH_SIZE);

        if (frame.result_case() != org::polypheny::prism::Frame::ResultCase::kRelationalFrame) {
            throw std::runtime_error("Statement returned a result of illegal type.");
        }

        is_fully_fetched = frame.is_last();
        add_rows(frame.relational_frame());
    }

    std::shared_ptr<RelationalMetadata> RelationalResult::get_metadata() const {
        return metadata;
    }

    RelationalResult::RowIterator RelationalResult::begin() {
        return {std::shared_ptr<RelationalResult>(this, [](RelationalResult*){}), 0};
    }

    RelationalResult::RowIterator RelationalResult::end() {
        return {std::shared_ptr<RelationalResult>(this, [](RelationalResult*){}), rows.size()};
    }

    RelationalResult::RowIterator::RowIterator(std::shared_ptr<RelationalResult> result, size_t index)
            : result(result), index(index) {}

    bool RelationalResult::RowIterator::operator==(const RowIterator &other) const {
        return index == other.index && result == other.result;
    }

    bool RelationalResult::RowIterator::operator!=(const RowIterator &other) const {
        return !(*this == other);
    }

    Row &RelationalResult::RowIterator::operator*() {
        return *std::next(result->rows.begin(), index);
    }

    RelationalResult::RowIterator &RelationalResult::RowIterator::operator++() {
        ++index;
        if (index >= result->rows.size() && !result->is_fully_fetched) {
            result->fetch_more();
        }
        return *this;
    }

} // Results