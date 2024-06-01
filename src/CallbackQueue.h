#ifndef POLYPHENY_CPP_DRIVER_CALLBACKQUEUE_H
#define POLYPHENY_CPP_DRIVER_CALLBACKQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <stdexcept>
#include "protointerface.pb.h"

namespace Communication {

    class ICallbackQueue {
    public:
        virtual ~ICallbackQueue() = default;

        virtual void await_completion() = 0;

        virtual void on_error(std::exception_ptr exception) = 0;

        virtual void on_completed() = 0;

        virtual void on_next(const org::polypheny::prism::Response &message) = 0;
    };

    template<typename T>
    class CallbackQueue : public ICallbackQueue {
    public:
        explicit CallbackQueue(std::function<const T &(const org::polypheny::prism::Response &)> &response_extractor);

        void await_completion() override;

        T take_next();

        void on_next(const org::polypheny::prism::Response &message) override;

        void on_error(std::exception_ptr exception) override;

        void on_completed() override;

    private:
        void throw_received_exception();

        std::mutex queue_mutex;
        std::condition_variable has_next;
        std::condition_variable is_completed;
        bool b_is_completed = false;
        std::queue<T> message_queue;
        std::function<T &(const org::polypheny::prism::Response &)> extract_response;
        std::exception_ptr propagated_exception;
    };

} // namespace Communication

#include "CallbackQueue.tpp"

#endif // POLYPHENY_CPP_DRIVER_CALLBACKQUEUE_H
