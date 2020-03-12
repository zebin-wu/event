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
     * @enum
    */
    enum Operation{
        OP_READ,        ///< read
        OP_WRITE,       ///< write
        OP_EXCEPTION,   ///< exception
    };

    /**
     * @brief
     *
     * @param handle
     * @param op
    */
    explicit HandleEvent(platform::Handle *handle,
        Operation op, common::Object *arg = nullptr):
        Event(arg), handle(handle), op(op) {}


    /**
	 * @brief Empty virtual destructor
	*/
    virtual ~HandleEvent() {}


    /**
     * @brief
     *
     * @return
    */
    platform::Handle *getHandle() const {
        return handle;
    }


    /**
     * @brief
     *
     * @param handle
    */
    void setHandle(platform::Handle *handle) {
        this->handle = handle;
    }


    /**
     * @brief
     *
     * @return
    */
    Operation getOperation() const {
        return op;
    }


    /**
     * @brief
     *
     * @return
    */
    const Callback<HandleEvent> *getCb() const {
        return cb;
    }


    /**
     * @brief
     *
     * @param cb
    */
    void setCb(const Callback<HandleEvent> *cb) {
        this->cb = cb;
    }


    /**
     * @brief
     *
     * @param op
    */
    void setOperation(Operation op) {
        this->op = op;
    }

 private:
    const Callback<HandleEvent> *cb;
    platform::Handle *handle;
    Operation op;
};

typedef common::ObjectException<HandleEvent> HandleEventException;

class HandleBus: public Bus<HandleEvent> {
 public:
    void addEvent(HandleEvent *e, const Callback<HandleEvent> *cb) override;

    void delEvent(HandleEvent *e) override;

    int dispatch(int timeout) override;

 private:
    platform::Poll::Event getEvent(HandleEvent::Operation op);
    platform::Poll poll;
};

}  // namespace event
