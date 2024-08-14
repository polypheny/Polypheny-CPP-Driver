#include "StringPrismInputStream.h"

namespace Streaming {

    StringPrismInputStream::StringPrismInputStream(const std::string &data, bool forward_only)
            : data(data), forward_only(forward_only), current_position(0), last_position(0), is_data_array(true),
              statement_id(0), stream_id(0), is_last(true) {
        setg(const_cast<char *>(data.data()), const_cast<char *>(data.data()),
             const_cast<char *>(data.data()) + data.size());
    }

    StringPrismInputStream::StringPrismInputStream(uint32_t statement_id, uint64_t stream_id, bool is_forward_only,
                                                   std::shared_ptr<Communication::PrismInterfaceClient> client)
            : forward_only(is_forward_only), current_position(0), last_position(0), is_data_array(false),
              statement_id(statement_id), stream_id(stream_id), client(std::move(client)), is_last(false) {
        setg(nullptr, nullptr, nullptr);
    }

    StringPrismInputStream::StringPrismInputStream(std::istream &input_stream, bool forward_only)
            : forward_only(forward_only), input_stream(&input_stream), current_position(0), last_position(0),
              is_data_array(false), is_last(false) {
        setg(nullptr, nullptr, nullptr);
    }

    std::streambuf::int_type StringPrismInputStream::underflow() {
        if (gptr() == egptr()) {
            if (is_last && current_position >= last_position + buffer.size()) {
                return traits_type::eof();
            }

            if (is_data_array) {
                if (current_position >= data.size()) {
                    return traits_type::eof();
                }
                setg(const_cast<char *>(data.data()), const_cast<char *>(data.data()) + current_position,
                     const_cast<char *>(data.data()) + data.size());
                gbump(static_cast<int>(current_position));
            } else if (input_stream) {
                buffer.resize(BUFFER_SIZE);
                input_stream->read(buffer.data(), BUFFER_SIZE);
                std::streamsize bytes_read = input_stream->gcount();
                buffer.resize(bytes_read);
                if (bytes_read == 0) {
                    return traits_type::eof();
                }
                setg(buffer.data(), buffer.data(), buffer.data() + buffer.size());
            } else {
                fetchBytes(current_position, BUFFER_SIZE);
                if (buffer.empty()) {
                    return traits_type::eof();
                }
                setg(buffer.data(), buffer.data(), buffer.data() + buffer.size());
            }
        }
        return traits_type::to_int_type(*gptr());
    }

    std::streambuf::int_type StringPrismInputStream::uflow() {
        int_type result = underflow();
        if (result != traits_type::eof()) {
            gbump(1);
            current_position++;
        }
        return result;
    }

    std::streamsize StringPrismInputStream::xsgetn(char_type *s, std::streamsize count) {
        std::streamsize bytes_read = 0;
        while (bytes_read < count) {
            std::streamsize available = egptr() - gptr();
            std::streamsize to_read = std::min(count - bytes_read, available);
            if (to_read > 0) {
                std::memcpy(s + bytes_read, gptr(), to_read);
                gbump(static_cast<int>(to_read));
                bytes_read += to_read;
                current_position += to_read;
            }

            if (bytes_read < count) {
                if (underflow() == traits_type::eof()) {
                    break;
                }
            }
        }
        return bytes_read;
    }

    std::streambuf::pos_type
    StringPrismInputStream::seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode which) {
        pos_type newpos = pos_type(off);
        if (way == std::ios_base::cur) {
            newpos = pos_type(current_position + (gptr() - eback()) + off);
        } else if (way == std::ios_base::end) {
            if (is_data_array) {
                newpos = pos_type(data.size() + off);
            } else {
                throw std::runtime_error("Seeking relative to the end is only supported for data arrays.");
            }
        } else if (way == std::ios_base::beg) {
            newpos = pos_type(off);
        }

        return seekpos(newpos, which);
    }

    std::streambuf::pos_type StringPrismInputStream::seekpos(pos_type position, std::ios_base::openmode which) {
        if (forward_only && position < pos_type(current_position)) {
            return pos_type(-1);
        }
        if (is_data_array) {
            if (position >= pos_type(data.size())) {
                return pos_type(-1);
            }
            current_position = position;
            setg(const_cast<char *>(data.data()), const_cast<char *>(data.data()) + current_position,
                 const_cast<char *>(data.data()) + data.size());
        } else {
            current_position = position;
            setg(nullptr, nullptr, nullptr);
        }
        return pos_type(current_position);
    }

    void StringPrismInputStream::fetchBytes(std::streamsize position, std::streamsize length) {
        if (forward_only && position < last_position + buffer.size()) {
            throw std::runtime_error("Cannot fetch data before the current position in forward-only mode.");
        }

        buffer.clear();
        org::polypheny::prism::StreamFrame frame = client->fetch_stream(statement_id, stream_id, position, length);
        is_last = frame.is_last();

        if (frame.data_case() != org::polypheny::prism::StreamFrame::kString) {
            throw std::runtime_error("Stream type must be text.");
        }

        buffer.assign(frame.string().begin(), frame.string().end());
        last_position = position;
    }

}  // namespace Types
