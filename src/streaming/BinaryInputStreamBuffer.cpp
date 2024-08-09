#include "streaming/BinaryInputStreamBuffer.h"
#include <stdexcept>
#include <cstring>

namespace Streaming {

    BinaryInputStreamBuffer::BinaryInputStreamBuffer(uint32_t statement_id, uint64_t stream_id,
                                                     std::shared_ptr<Communication::PrismInterfaceClient> client)
            : statement_id(statement_id),
              stream_id(stream_id),
              client(std::move(client)),
              buffer(BUFFER_SIZE),
              buffer_start_position(0),
              is_last(false) {
        setg(buffer.data(), buffer.data(), buffer.data());
    }

    void BinaryInputStreamBuffer::fetchNextBytes() {
        if (is_last) return;

        std::streamsize fetch_position = buffer_start_position + buffer.size();
        org::polypheny::prism::StreamFrame frame = client->fetch_stream(statement_id, stream_id, fetch_position,
                                                                        BUFFER_SIZE);

        is_last = frame.is_last();
        buffer_start_position = fetch_position;

        if (frame.data_case() != org::polypheny::prism::BINARY) {
            throw std::runtime_error("Stream type must be binary.");
        }

        buffer.assign(frame.binary().begin(), frame.binary().end());
        setg(buffer.data(), buffer.data(), buffer.data() + buffer.size());
    }

    BinaryInputStreamBuffer::int_type BinaryInputStreamBuffer::underflow() {
        if (gptr() == egptr()) {
            fetchNextBytes();
            if (gptr() == egptr()) {
                return traits_type::eof();
            }
        }
        return traits_type::to_int_type(*gptr());
    }

    BinaryInputStreamBuffer::int_type BinaryInputStreamBuffer::uflow() {
        int_type result = underflow();
        if (result != traits_type::eof()) {
            gbump(1);
        }
        return result;
    }

    std::streamsize BinaryInputStreamBuffer::xsgetn(char_type *s, std::streamsize count) {
        std::streamsize bytes_read = 0;
        while (bytes_read < count) {
            std::streamsize available = egptr() - gptr();
            std::streamsize to_read = std::min(count - bytes_read, available);
            std::memcpy(s + bytes_read, gptr(), to_read);
            gbump(static_cast<int>(to_read));
            bytes_read += to_read;

            if (bytes_read < count) {
                fetchNextBytes();
                if (gptr() == egptr()) {
                    break;  // no more data available
                }
            }
        }
        return bytes_read;
    }

}  // namespace Streaming
