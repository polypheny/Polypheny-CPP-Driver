#ifndef POLYPHENY_CPP_DRIVER_STATEMENTSTATE_H
#define POLYPHENY_CPP_DRIVER_STATEMENTSTATE_H

#include "PrismInterfaceClient.h"

namespace Communication {

    // A cursor and its outstanding result share one server-side statement.
    // The final owner closes it, even when the result outlives its cursor.
    struct StatementState {
        std::shared_ptr<PrismInterfaceClient> client;
        uint32_t id;
        bool closed = false;

        StatementState(std::shared_ptr<PrismInterfaceClient> client, uint32_t id)
                : client(std::move(client)), id(id) {}

        ~StatementState() {
            try {
                close();
            } catch (const std::exception &) {
            }
        }

        void close() {
            if (!closed) {
                closed = true;
                client->close_statement(id);
            }
        }
    };

}

#endif
