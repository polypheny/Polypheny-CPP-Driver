#include "Result.h"

namespace Results {

    Result::Result(ResultType type, std::shared_ptr<Communication::PrismInterfaceClient> client,
                   uint32_t statement_id, uint32_t fetch_size)
            : resultType(type), client(std::move(client)), statement_id(statement_id), fetch_size(fetch_size) {}

    Result::~Result() {
        try {
            close();
        } catch (const std::exception &) {
            // Explicit close() reports failures. Destruction still releases local state.
        }
    }

    void Result::close() {
        if (closed) {
            return;
        }
        closed = true;
        if (client) {
            client->close_result(statement_id);
        }
    }

    org::polypheny::prism::Frame Result::fetch_frame() {
        if (closed) {
            throw std::runtime_error("The result is closed");
        }
        try {
            auto frame = client->fetch_result(statement_id, fetch_size);
            const bool expected = (resultType == ResultType::GRAPH && frame.has_graph_frame())
                    || (resultType == ResultType::RELATIONAL && frame.has_relational_frame())
                    || (resultType == ResultType::DOCUMENT && frame.has_document_frame());
            if (!expected) {
                throw std::runtime_error("Prism returned an unexpected result frame type");
            }
            return frame;
        } catch (...) {
            try {
                close();
            } catch (const std::exception &) {
            }
            throw;
        }
    }

}
