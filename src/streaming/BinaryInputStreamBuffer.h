#ifndef POLYPHENY_CPP_DRIVER_BINARYINPUTSTREAMBUFFER_H
#define POLYPHENY_CPP_DRIVER_BINARYINPUTSTREAMBUFFER_H

#include <streambuf>
#include <vector>
#include "connection/PrismInterfaceClient.h"

namespace Streaming {

    class BinaryInputStreamBuffer : public std::streambuf {
    public:
        static const int BUFFER_SIZE = 10000;

        BinaryInputStreamBuffer(uint32_t statement_id, uint64_t stream_id,
                                std::shared_ptr<Communication::PrismInterfaceClient> client);

    protected:
        int_type underflow() override;
        int_type uflow() override;
        std::streamsize xsgetn(char_type* s, std::streamsize count) override;

    private:
        void fetchNextBytes();

        uint32_t statement_id;
        uint64_t stream_id;
        std::shared_ptr<Communication::PrismInterfaceClient> client;

        std::vector<char> buffer;
        std::streamsize buffer_start_position;
        bool is_last;
    };

}  // namespace Streaming

#endif //POLYPHENY_CPP_DRIVER_BINARYINPUTSTREAMBUFFER_H
