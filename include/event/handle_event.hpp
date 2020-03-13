/**
 * Copyright (c) 2020 KNpTrue
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
#pragma once

#include <event/event.hpp>
#include <event/bus.hpp>
#include <platform/poll.hpp>

/**
 * @file handle_event.hpp
 * @brief Handle event interfaces
*/

namespace event {

class HandleEvent: public Event {
 public:
    /**
     * @enum The operation of the handle event
    */
    enum Operation{
        OP_READ,        ///< read
        OP_WRITE,       ///< write
        OP_EXCEPTION,   ///< exception
    };

    /**
     * @brief Default constructor
     *
     * @param handle A point to the handle
     * @param op The operation of the handle event
    */
    explicit HandleEvent(platform::Handle *handle,
        Operation op):
        handle(handle), op(op) {}


    /**
     * @brief Empty virtual destructor
    */
    virtual ~HandleEvent() {}


    /**
     * @brief Get the handle
     *
     * @return the point of the handle
    */
    platform::Handle *getHandle() const {
        return handle;
    }


    /**
     * @brief Set the handle
     *
     * @param handle A point to the handle
    */
    void setHandle(platform::Handle *handle) {
        this->handle = handle;
    }


    /**
     * @brief Get the operation
     *
     * @return the operation
    */
    Operation getOperation() const {
        return op;
    }


    /**
     * @brief Set the operation
     *
     * @param op is the operation of the event
    */
    void setOperation(Operation op) {
        this->op = op;
    }


    /**
     * @brief Get the callback of the event
     *
     * @return a point of the event
    */
    const Callback<HandleEvent> *getCb() const {
        return cb;
    }


    /**
     * @brief Set the callback of the event
     *
     * @param cb is a const point of the event
    */
    void setCb(const Callback<HandleEvent> *cb) {
        this->cb = cb;
    }

 private:
    const Callback<HandleEvent> *cb;
    platform::Handle *handle;
    Operation op;
};

typedef common::ObjectException<HandleEvent> HandleEventException;

class HandleBus: public Bus<HandleEvent> {
 public:
    /**
     * @brief Override to add a handle event to the handle bus 
    */
    void addEvent(HandleEvent *e, const Callback<HandleEvent> *cb) override;


    /**
     * @brief Override to delete a handle event from the handle bus 
    */
    void delEvent(HandleEvent *e) override;


    /**
     * @brief Override to trigger the event
     *
     * @param timeout Specifies the maximum wait time in milliseconds(-1 == infinite)
     *
     * @return Specifies the maximum wait time to return @c dispatch(), -1 if it do not need to wait
    */
    int dispatch(int timeout) override;

 private:
    platform::Poll::Event getEvent(HandleEvent::Operation op);
    platform::Poll poll;
};

}  // namespace event
