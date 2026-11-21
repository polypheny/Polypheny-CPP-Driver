#include "RelationalResult.h"

#include "src/connection/Cursor.h"
#include "src/connection/Connection.h"
#include <limits>

namespace Results {
    RelationalResult::RelationalResult(const org::polypheny::prism::RelationalFrame &relational_frame,
                                       bool is_last, Connection::Cursor *cursor)
            : Result(ResultType::RELATIONAL, cursor->get_connection().get_prism_interface_client(),
                     cursor->get_statement_id(), cursor->get_fetch_size()),
              is_fully_fetched(is_last) {
        metadata = std::make_shared<RelationalMetadata>(relational_frame);
        add_rows(relational_frame);
    }

    void RelationalResult::add_rows(const org::polypheny::prism::RelationalFrame &relationalFrame) {
        for (const auto &row : relationalFrame.rows()) {
            rows.emplace_back(row, metadata, client);
        }
    }

    void RelationalResult::fetch_more() {
        const auto frame = fetch_frame();

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
        ensure_available(0);
        return {std::shared_ptr<RelationalResult>(this, [](RelationalResult*){}), 0};
    }

    RelationalResult::RowIterator RelationalResult::end() {
        return {std::shared_ptr<RelationalResult>(this, [](RelationalResult*){}), std::numeric_limits<size_t>::max()};
    }

    void RelationalResult::ensure_available(size_t index) {
        if (is_closed()) {
            throw std::runtime_error("The result is closed");
        }
        while (index >= rows.size() && !is_fully_fetched) {
            fetch_more();
        }
    }

    RelationalResult::RowIterator::RowIterator(std::shared_ptr<RelationalResult> result, size_t index)
            : result(result), index(index) {}

    bool RelationalResult::RowIterator::operator==(const RowIterator &other) const {
        if (result != other.result) {
            return false;
        }
        const auto sentinel = std::numeric_limits<size_t>::max();
        if (index == sentinel || other.index == sentinel) {
            const auto position = index == sentinel ? other.index : index;
            return position == sentinel || (result->is_fully_fetched && position >= result->rows.size());
        }
        return index == other.index;
    }

    bool RelationalResult::RowIterator::operator!=(const RowIterator &other) const {
        return !(*this == other);
    }

    Row &RelationalResult::RowIterator::operator*() {
        result->ensure_available(index);
        if (index >= result->rows.size()) {
            throw std::out_of_range("Relational result iterator is at the end");
        }
        return *std::next(result->rows.begin(), index);
    }

    RelationalResult::RowIterator &RelationalResult::RowIterator::operator++() {
        ++index;
        result->ensure_available(index);
        return *this;
    }

} // Results
