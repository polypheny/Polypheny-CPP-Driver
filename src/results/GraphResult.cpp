//
// Created by tobi on 01.06.24.
//

#include "GraphResult.h"
#include "results/Node.h"
#include "results/Edge.h"
#include "connection/Cursor.h"
#include "connection/Connection.h"
#include <limits>

namespace Results {
    GraphResult::GraphResult(const org::polypheny::prism::GraphFrame &graph_frame, bool is_last, Connection::Cursor *cursor)
            : Result(ResultType::GRAPH, cursor->get_connection().get_prism_interface_client(),
                     cursor->get_statement_id(), cursor->get_fetch_size()),
              is_fully_fetched(is_last) {
        add_graph_elements(graph_frame);
    }

    void GraphResult::add_graph_elements(const org::polypheny::prism::GraphFrame &graph_frame) {
        for (const auto &element : graph_frame.element()) {
            switch(element.element_case()) {
                case (org::polypheny::prism::GraphElement::kNode):
                    elements.push_back(std::make_shared<Node>(element.node(), client));
                    break;
                case (org::polypheny::prism::GraphElement::kEdge):
                    elements.push_back(std::make_shared<Edge>(element.edge(), client));
                    break;
                default:
                    throw std::runtime_error("Graph frame contains an unknown or empty element");
            }
        }
    }

    void GraphResult::fetch_more() {
        const auto frame = fetch_frame();

        if (frame.result_case() != org::polypheny::prism::Frame::ResultCase::kGraphFrame) {
            throw std::runtime_error("Statement returned a result of illegal type.");
        }

        is_fully_fetched = frame.is_last();
        try {
            add_graph_elements(frame.graph_frame());
        } catch (...) {
            try {
                close();
            } catch (const std::exception &) {
            }
            throw;
        }
    }

    GraphResult::GraphElementIterator GraphResult::begin() {
        ensure_available(0);
        return {std::shared_ptr<GraphResult>(this, [](GraphResult*){}), 0};
    }

    GraphResult::GraphElementIterator GraphResult::end() {
        return {std::shared_ptr<GraphResult>(this, [](GraphResult*){}), std::numeric_limits<size_t>::max()};
    }

    void GraphResult::ensure_available(size_t index) {
        if (closed) {
            throw std::runtime_error("The result is closed");
        }
        while (index >= elements.size() && !is_fully_fetched) {
            fetch_more();
        }
    }

    GraphResult::GraphElementIterator::GraphElementIterator(std::shared_ptr<GraphResult> result, size_t index)
            : result(std::move(result)), index(index) {}

    bool GraphResult::GraphElementIterator::operator==(const GraphElementIterator &other) const {
        if (result != other.result) {
            return false;
        }
        const auto sentinel = std::numeric_limits<size_t>::max();
        if (index == sentinel || other.index == sentinel) {
            const auto position = index == sentinel ? other.index : index;
            return position == sentinel || (result->is_fully_fetched && position >= result->elements.size());
        }
        return index == other.index;
    }

    bool GraphResult::GraphElementIterator::operator!=(const GraphElementIterator &other) const {
        return !(*this == other);
    }

    std::shared_ptr<GraphElement> GraphResult::GraphElementIterator::operator*() {
        result->ensure_available(index);
        return result->elements.at(index);
    }

    GraphResult::GraphElementIterator &GraphResult::GraphElementIterator::operator++() {
        ++index;
        result->ensure_available(index);
        return *this;
    }
} // Results
