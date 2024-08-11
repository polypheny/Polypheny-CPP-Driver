//
// Created by tobi on 11.08.24.
//

#ifndef POLYPHENY_CPP_DRIVER_STRINGPRISMOUTPUTSTREAM_H
#define POLYPHENY_CPP_DRIVER_STRINGPRISMOUTPUTSTREAM_H

#include "PrismOutputStream.h"
#include <string>
#include <memory>
#include "connection/PrismInterfaceClient.h"

namespace Streaming {

    class StringPrismOutputStream : public PrismOutputStream {
    public:
        explicit StringPrismOutputStream(std::string  varcharValue);

        void build_and_run(uint32_t statementId, uint64_t streamId,
                         std::shared_ptr<Communication::PrismInterfaceClient> prismInterfaceClient) override;

        void run() override;

    private:
        std::string data;
        std::shared_ptr<Communication::PrismInterfaceClient> client;
    };

} // namespace Streaming

#endif //POLYPHENY_CPP_DRIVER_STRINGPRISMOUTPUTSTREAM_H
