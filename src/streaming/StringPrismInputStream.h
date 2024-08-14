//
// Created by tobi on 14.08.24.
//

#ifndef POLYPHENY_CPP_DRIVER_STRINGPRISMINPUTSTREAM_H
#define POLYPHENY_CPP_DRIVER_STRINGPRISMINPUTSTREAM_H

#include <streambuf>
#include <vector>
#include <memory>
#include <string>
#include "connection/PrismInterfaceClient.h"

namespace Streaming {

    class StringPrismInputStream : public std::streambuf {
    public:
        StringPrismInputStream(const std::string &data, bool forward_only);

        StringPrismInputStream(uint32_t statement_id, uint64_t stream_id, bool is_forward_only,
                               std::shared_ptr<Communication::PrismInterfaceClient> client);

        StringPrismInputStream(std::istream &input_stream, bool forward_only);

        static const std::streamsize BUFFER_SIZE = 10000;

    protected:
        std::streambuf::int_type underflow() override;

        std::streambuf::int_type uflow() override;

        std::streamsize xsgetn(char_type *s, std::streamsize count) override;

        std::streambuf::pos_type
        seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode which) override;

        std::streambuf::pos_type seekpos(pos_type position, std::ios_base::openmode which) override;

    private:
        void fetchBytes(std::streamsize position, std::streamsize length);

        std::string data;
        std::shared_ptr<std::istream> input_stream;
        std::vector<char> buffer;
        std::shared_ptr<Communication::PrismInterfaceClient> client;
        uint32_t statement_id;
        uint64_t stream_id;
        std::streamsize current_position;
        std::streamsize last_position;
        bool forward_only;
        bool is_data_array;
        bool is_last;
    };

}  // namespace Types

#endif //POLYPHENY_CPP_DRIVER_STRINGPRISMINPUTSTREAM_H
