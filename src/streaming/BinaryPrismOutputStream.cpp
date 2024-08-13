//
// Created by tobi on 10.08.24.
//

#include "BinaryPrismOutputStream.h"

#include <stdexcept>
#include <algorithm>
#include <cstring>

namespace Streaming {

    BinaryPrismOutputStream::BinaryPrismOutputStream(std::vector<uint8_t> &data)
            : data(data) {
    }

    void BinaryPrismOutputStream::build_and_run(uint32_t statement_id, uint64_t stream_id,
                                                std::shared_ptr<Communication::PrismInterfaceClient> prism_interface_client) {
        set_statement_id(statement_id);
        set_stream_id(stream_id);
        this->client = std::move(prism_interface_client);
        streaming_thread = std::thread(&BinaryPrismOutputStream::run, this);
    }

    void BinaryPrismOutputStream::run() {
        if (!client) {
            throw std::runtime_error("PrismInterfaceClient not set");
        }

        size_t size = data.size();
        size_t offset = 0;

        while (offset < size && !stop_flag.load()) {
            size_t frame_size = std::min(static_cast<size_t>(Types::TypedValue::STREAMING_THRESHOLD), size - offset);
            std::vector<uint8_t> frame_data(frame_size);
            std::memcpy(frame_data.data(), data.data() + offset, frame_size);
            bool isLast = (offset + frame_size) >= size;

            auto ack = client->stream_binary(frame_data, isLast, statement_id.load(), stream_id.load());
            if (ack.close_stream()) {
                return;
            }

            offset += frame_size;
        }
    }
} // namespace Streaming
