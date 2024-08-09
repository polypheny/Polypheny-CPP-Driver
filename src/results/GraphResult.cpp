//
// Created by tobi on 01.06.24.
//

#include "GraphResult.h"
#include "results/Node.h"
#include "results/Edge.h"
#include "connection/Cursor.h"
#include "connection/Connection.h"
#include "Path.h"

namespace Results {
    GraphResult::GraphResult(const org::polypheny::prism::GraphFrame &graph_frame, bool is_last, Connection::Cursor *cursor)
            : Result(ResultType::GRAPH), is_fully_fetched(is_last), cursor(cursor) {
        add_graph_elements(graph_frame);
    }

    GraphResult::~GraphResult() {
        uint32_t statement_id = cursor->get_statement_id();
        cursor->get_connection().get_prism_interface_client().close_result(statement_id);
    }

    void GraphResult::add_graph_elements(const org::polypheny::prism::GraphFrame &graph_frame) {
        for (const auto &element : graph_frame.element()) {
            switch(element.element_case()) {
                case (org::polypheny::prism::GraphElement::kNode):
                    elements.push_back(std::make_shared<Node>(element.node()));
                    break;
                case (org::polypheny::prism::GraphElement::kEdge):
                    elements.push_back(std::make_shared<Edge>(element.edge()));
                    break;
            }
        }
    }

    void GraphResult::fetch_more() {
        uint32_t statement_id = cursor->get_statement_id();
        org::polypheny::prism::Frame frame = cursor->get_connection().get_prism_interface_client().fetch_result(
                statement_id, DEFAULT_FETCH_SIZE);

        if (frame.result_case() != org::polypheny::prism::Frame::ResultCase::kGraphFrame) {
            throw std::runtime_error("Statement returned a result of illegal type.");
        }

        is_fully_fetched = frame.is_last();
        add_graph_elements(frame.graph_frame());
    }

    GraphResult::GraphElementIterator GraphResult::begin() {
        return {std::shared_ptr<GraphResult>(this, [](GraphResult*){}), 0};
    }

    GraphResult::GraphElementIterator GraphResult::end() {
        return {std::shared_ptr<GraphResult>(this, [](GraphResult*){}), elements.size()};
    }

    GraphResult::GraphElementIterator::GraphElementIterator(std::shared_ptr<GraphResult> result, size_t index)
            : result(std::move(result)), index(index) {}

    bool GraphResult::GraphElementIterator::operator==(const GraphElementIterator &other) const {
        return index == other.index && result == other.result;
    }

    bool GraphResult::GraphElementIterator::operator!=(const GraphElementIterator &other) const {
        return !(*this == other);
    }

    std::shared_ptr<GraphElement> GraphResult::GraphElementIterator::operator*() {
        return result->elements[index];
    }

    GraphResult::GraphElementIterator &GraphResult::GraphElementIterator::operator++() {
        ++index;
        if (index >= result->elements.size() && !result->is_fully_fetched) {
            result->fetch_more();
        }
        return *this;
    }
} // Results