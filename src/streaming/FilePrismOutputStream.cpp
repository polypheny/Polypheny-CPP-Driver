#include "FilePrismOutputStream.h"
#include "types/TypedValue.h"
#include <iostream>

namespace Streaming {

    FilePrismOutputStream::FilePrismOutputStream(std::shared_ptr<std::streambuf> fileStream)
            : fileStream(std::move(fileStream)) {}

    void FilePrismOutputStream::build_and_run(uint32_t statement_id, uint64_t stream_id,
                                              std::shared_ptr<Communication::PrismInterfaceClient> prism_interface_client) {
        set_statement_id(statement_id);
        set_stream_id(stream_id);
        this->client = std::move(prism_interface_client);
        streaming_thread = std::thread(&FilePrismOutputStream::run, this);
    }

    void FilePrismOutputStream::run() {
        if (!client) {
            throw std::runtime_error("PrismInterfaceClient not set");
        }

        std::vector<char> buffer(Types::TypedValue::STREAMING_THRESHOLD);
        uint64_t offset = 0;
        while (true) {
            std::streamsize bytesRead = fileStream->sgetn(buffer.data(), Types::TypedValue::STREAMING_THRESHOLD);
            if (bytesRead == 0) {
                break;
            }
            bool isLast = (bytesRead < Types::TypedValue::STREAMING_THRESHOLD);
            std::vector<uint8_t> frameData(buffer.begin(), buffer.begin() + bytesRead);
            auto ack = client->stream_binary(frameData, isLast, statement_id, stream_id);
            if (ack.close_stream()) {
                return;
            }
            offset += bytesRead;
        }
    }

} // namespace Streaming
