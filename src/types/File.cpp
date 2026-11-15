#include "File.h"

namespace Types {

    File::File(const std::vector<uint8_t>& data, bool forward_only)
            : data(data), is_data_array(true), forward_only(forward_only), current_position(0), last_position(0) {
        if (!this->data.empty()) {
            auto *start = reinterpret_cast<char *>(this->data.data());
            setg(start, start, start + this->data.size());
        }
    }

    File::File(uint32_t statement_id, uint64_t stream_id, bool is_forward_only,
               std::shared_ptr<Communication::PrismInterfaceClient> client)
            : client(std::move(client)), statement_id(statement_id), stream_id(stream_id),
              forward_only(is_forward_only), current_position(0), last_position(0) {
        setg(nullptr, nullptr, nullptr);
    }

    File::File(std::istream& input_stream, bool forward_only)
            : forward_only(forward_only), current_position(0), last_position(0), input_stream(&input_stream) {
        setg(nullptr, nullptr, nullptr);
    }

    File::File(const File &other) : current_position(0), last_position(0) {
        *this = other;
    }

    File &File::operator=(const File &other) {
        if (this == &other) {
            return *this;
        }
        auto copied_data = other.data;
        auto copied_buffer = other.buffer;
        data = std::move(copied_data);
        buffer = std::move(copied_buffer);
        client = other.client;
        statement_id = other.statement_id;
        stream_id = other.stream_id;
        is_data_array = other.is_data_array;
        forward_only = other.forward_only;
        is_last = other.is_last;
        current_position = other.current_position;
        last_position = other.last_position;
        input_stream = other.input_stream;
        if (other.eback()) {
            auto *start = reinterpret_cast<char *>((is_data_array ? data : buffer).data());
            setg(start, start + (other.gptr() - other.eback()), start + (other.egptr() - other.eback()));
        } else {
            setg(nullptr, nullptr, nullptr);
        }
        return *this;
    }

    std::streambuf::int_type File::underflow() {
        if (is_data_array) {
            return gptr() == egptr() ? traits_type::eof() : traits_type::to_int_type(*gptr());
        }
        if (gptr() == egptr()) {
            if (is_last && current_position >= last_position + buffer.size()) {
                return traits_type::eof();
            }

            if (input_stream) {
                buffer.resize(BUFFER_SIZE);
                input_stream->read(reinterpret_cast<char*>(buffer.data()), BUFFER_SIZE);
                std::streamsize bytes_read = input_stream->gcount();
                buffer.resize(bytes_read);
                if (bytes_read == 0) {
                    return traits_type::eof();
                }
                setg(reinterpret_cast<char*>(buffer.data()), reinterpret_cast<char*>(buffer.data()), reinterpret_cast<char*>(buffer.data()) + buffer.size());
            } else {
                fetchBytes(current_position, BUFFER_SIZE);
                if (buffer.empty()) {
                    return traits_type::eof();
                }
                setg(reinterpret_cast<char*>(buffer.data()), reinterpret_cast<char*>(buffer.data()), reinterpret_cast<char*>(buffer.data()) + buffer.size());
            }
        }
        return traits_type::to_int_type(*gptr());
    }

    std::streambuf::int_type File::uflow() {
        int_type result = underflow();
        if (result != traits_type::eof()) {
            gbump(1);
            current_position++;
        }
        return result;
    }

    std::streamsize File::xsgetn(char_type* s, std::streamsize count) {
        std::streamsize bytes_read = 0;
        while (bytes_read < count) {
            std::streamsize available = gptr() ? egptr() - gptr() : 0;
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

    std::streambuf::pos_type File::seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode which) {
        pos_type newpos = pos_type(off);
        if (way == std::ios_base::cur) {
            const auto offset = gptr() ? gptr() - eback() : 0;
            const auto position = is_data_array ? offset : current_position + offset;
            newpos = pos_type(position + off);
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

    std::streambuf::pos_type File::seekpos(pos_type position, std::ios_base::openmode which) {
        const auto current = is_data_array && gptr() ? gptr() - eback() : current_position;
        if (position < pos_type(0) || (forward_only && position < pos_type(current))) {
            return pos_type(-1);
        }
        if (is_data_array) {
            if (position > pos_type(data.size())) {
                return pos_type(-1);
            }
            current_position = position;
            if (!data.empty()) {
                auto *start = reinterpret_cast<char *>(data.data());
                setg(start, start + current_position, start + data.size());
            }
        } else {
            current_position = position;
            setg(nullptr, nullptr, nullptr);  // Invalidate the buffer pointers to force a reload
        }
        return pos_type(current_position);
    }

    void File::fetchBytes(std::streamsize position, std::streamsize length) {
#if POLYPHENY_PRISM_STREAMING
        if (forward_only && position < last_position + buffer.size()) {
            throw std::runtime_error("Cannot fetch data before the current position in forward-only mode.");
        }

        buffer.clear();
        org::polypheny::prism::StreamFrame frame = client->fetch_stream(statement_id, stream_id, position, length);
        is_last = frame.is_last();

        if (frame.data_case() != org::polypheny::prism::StreamFrame::kBinary) {
            throw std::runtime_error("Stream type must be binary.");
        }

        buffer.assign(frame.binary().begin(), frame.binary().end());
        last_position = position;
#else
        throw std::logic_error("Prism 2.2 does not support remote file streams");
#endif
    }
}  // namespace Types
