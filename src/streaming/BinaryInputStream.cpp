#include "streaming/BinaryInputStream.h"

namespace Streaming {

    BinaryInputStream::BinaryInputStream(uint32_t statement_id, uint64_t stream_id,
                                         std::shared_ptr<Communication::PrismInterfaceClient> client)
            : std::istream(&buffer_),
              buffer_(statement_id, stream_id, std::move(client)) {
    }

}  // namespace Streaming
