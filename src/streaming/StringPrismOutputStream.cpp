#include "StringPrismOutputStream.h"

#include <utility>
#include "connection/PrismInterfaceClient.h"
#include <algorithm>

namespace Streaming {

    StringPrismOutputStream::StringPrismOutputStream(std::string varcharValue)
            : data(std::move(varcharValue)) {}

    void StringPrismOutputStream::build_and_run(uint32_t statementId, uint64_t streamId,
                                                std::shared_ptr<Communication::PrismInterfaceClient> prismInterfaceClient) {
        set_statement_id(statementId);
        set_stream_id(streamId);
        client = std::move(prismInterfaceClient);
        run();
    }

    void StringPrismOutputStream::run() {
        if (!client) {
            throw std::runtime_error("PrismInterfaceClient not set");
        }

        size_t size = data.length();
        size_t offset = 0;

        while (offset < size) {
            size_t frame_size = std::min(static_cast<size_t>(Types::TypedValue::STREAMING_THRESHOLD), size - offset);
            std::string frame_data = data.substr(offset, frame_size);
            bool isLast = (offset + frame_size) >= size;

            auto ack = client->stream_string(frame_data, isLast, statement_id, stream_id);
            if (ack.close_stream()) {
                return;
            }
            offset += frame_size;
        }
    }

} // namespace Streaming
