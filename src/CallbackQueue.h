#ifndef POLYPHENY_CPP_DRIVER_CALLBACKQUEUE_H
#define POLYPHENY_CPP_DRIVER_CALLBACKQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <stdexcept>
#include "protointerface.pb.h"

namespace Communication {

    class CallbackQueue {
    public:
        void await_completion();

        org::polypheny::prism::Response take_next();

        void on_next(const org::polypheny::prism::Response &message);

        void on_error(std::exception_ptr exception);

        void on_completed();

    private:
        void throw_received_exception();

        std::mutex queue_mutex;
        std::condition_variable has_next;
        std::condition_variable is_completed;
        bool b_is_completed = false;
        std::queue<org::polypheny::prism::Response> message_queue;
        std::function<const org::polypheny::prism::Response &(const org::polypheny::prism::Response &)> extract_response;
        std::exception_ptr propagated_exception;
    };

} // namespace Communication

#endif // POLYPHENY_CPP_DRIVER_CALLBACKQUEUE_H
