//
// Created by tobi on 10.08.24.
//

#include "StreamingIndex.h"

namespace Streaming {

    StreamingIndex::StreamingIndex(std::shared_ptr<Communication::PrismInterfaceClient> prism_interface_client)
            : prism_interface_client(std::move(prism_interface_client)), stream_id_generator(0) {}

    int64_t StreamingIndex::register_stream(const std::shared_ptr<PrismOutputStream> &stream) {
        streams.insert(stream);
        long stream_id = stream_id_generator.fetch_add(1);
        stream->build_and_run(statement_id, stream_id, prism_interface_client);
        return stream_id;
    }

    void StreamingIndex::update(uint32_t statement_id, bool is_statement_id_set) {
        this->statement_id = statement_id;
        this->is_statement_id_set = is_statement_id_set;
    }

} // namespace Streaming
