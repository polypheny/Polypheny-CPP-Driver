#ifndef POLYPHENY_CPP_DRIVER_BINARYINPUTSTREAM_H
#define POLYPHENY_CPP_DRIVER_BINARYINPUTSTREAM_H

#include "streaming/BinaryInputStreamBuffer.h"
#include "connection/PrismInterfaceClient.h"
#include <memory>

namespace Streaming {

    class BinaryInputStream : public std::istream {
    public:
        BinaryInputStream(uint32_t statement_id, uint64_t stream_id,
                          std::shared_ptr<Communication::PrismInterfaceClient> client);

    private:
        BinaryInputStreamBuffer buffer_;
    };

}  // namespace Streaming

#endif //POLYPHENY_CPP_DRIVER_BINARYINPUTSTREAM_H
