#ifndef POLYPHENY_CPP_DRIVER_FILE_H
#define POLYPHENY_CPP_DRIVER_FILE_H

#include <streambuf>
#include <vector>
#include <memory>
#include <stdexcept>
#include <cstring>

#include "connection/PrismInterfaceClient.h"

namespace Types {

    class File : public std::streambuf {
    public:
        explicit File(const std::vector<uint8_t>& data, bool forward_only = false);

        File(uint32_t statement_id, uint64_t stream_id, bool is_forward_only,
             std::shared_ptr<Communication::PrismInterfaceClient> client);

        explicit File(std::istream& input_stream, bool forward_only = false);

        static const std::streamsize BUFFER_SIZE = 10000;

    protected:
        int_type underflow() override;  // For reading (input)
        int_type uflow() override;      // For reading and advancing the input position
        std::streamsize xsgetn(char_type* s, std::streamsize count) override;

        pos_type seekoff(off_type off, std::ios_base::seekdir way,
                         std::ios_base::openmode which) override;

        pos_type seekpos(pos_type position, std::ios_base::openmode which) override;

    private:
        std::shared_ptr<Communication::PrismInterfaceClient> client;

        uint32_t statement_id;
        uint64_t stream_id;

        std::vector<uint8_t> data;
        std::vector<uint8_t> buffer;

        bool is_data_array = false;
        bool forward_only = false;
        bool is_last = false;

        std::streamsize current_position;
        std::streamsize last_position;

        std::istream* input_stream = nullptr;

        void fetchBytes(std::streamsize position, std::streamsize length);
        void updatePosition(std::streamsize new_position);
    };

}  // namespace Types

#endif // POLYPHENY_CPP_DRIVER_FILE_H
