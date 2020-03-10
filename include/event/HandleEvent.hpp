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

#include <event/Event.hpp>
#include <event/Base.hpp>
#include <platform/poll.hpp>

/**
 * @file TimerEvent.hpp
 * @brief Event base class
*/

namespace event {

typedef common::ClassException<HandleEvent> HandleEventException;

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
    explicit HandleEvent(platform::Handle & handle, Operation op);


    /**
     * @brief
    */
    virtual ~HandleEvent() {}


    /**
     * @brief
     *
     * @return
    */
    platform::Handle & getHandle() const {
        return handle;
    }


    /**
     * @brief
     *
     * @param handle
    */
    void setHandle(platform::Handle & handle) {
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
    Callback<HandleEvent> & getCb() const {
        return cb;
    }


    /**
     * @brief
     *
     * @param cb
    */
    void setCb(Callback<HandleEvent> & cb) {
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
    Callback<HandleEvent> & cb;
    platform::Handle & handle;
    Operation op;
};

class HandleBase: public Base<HandleEvent> {
 public:
    void addEvent(HandleEvent & e, Callback<HandleEvent> & cb) override {
        if (e.isPending()) {
            return;
        }
        e.setPending(true);
        poll.add(e.getHandle(), getEvent(e.getOperation()),
            [] (platform::Poll::Event mode,
                platform::Handle & handle, void *arg) {
                HandleEvent *e = static_cast<HandleEvent *>(arg);
                e->getCb().onEvent(*e);
            }, &e);
    }

    void delEvent(HandleEvent & e) override {
        if (!e.isPending()) {
            return;
        }
        poll.del(e.getHandle(), getEvent(e.getOperation()));
    }

    int dispatch(int ms) override {
        poll.polling(ms);
    }

 private:
    platform::Poll::Event getEvent(HandleEvent::Operation op) {
        platform::Poll::Event e;
        switch (op) {
        case HandleEvent::OP_READ:
            e = platform::Poll::EV_READ;
            break;
        case HandleEvent::OP_WRITE:
            e = platform::Poll::EV_WRITE;
            break;
        case HandleEvent::OP_EXCEPTION:
            e = platform::Poll::EV_ERR;
            break;
        }
        return e;
    }
    platform::Poll poll;
};

}  // namespace event