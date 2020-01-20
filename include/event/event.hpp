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

#include <common/error.hpp>

/**
 * @file event.hpp
 * @brief Event class
 */

using namespace common;

namespace event {

class Callback;
class SignalCb;
class TimerCb;
class HandleCb;

class Event {
public:
    enum Type {
        EV_HANDLE,  ///< Socket or FD event
        EV_TIMER,   ///< Timer event
        EV_SIGNAL,  ///< POSIX signal event
    };

    Event(Type type, Callback *cb);
    ~Event();

    Type getType() const;
    ErrorCode setType(Type type);
    Callback *getCb() const;
    ErrorCode setCb(Callback *cb);
private:
    Type type;
    Callback *cb;
};

class SignalEvent: public Event {
public:
    enum Signal {
        SIGNAL_INT,
    };
    SignalEvent(SignalCb *cb, Signal signal);
    ~SignalEvent();

    Signal getSignal() const;
    ErrorCode setSignal(Signal signal);
private:
    Signal signal;
};

class TimerEvent: public Event {
public:
    typedef unsigned long time_t;
    TimerEvent(TimerCb *cb, time_t timeout);
    ~TimerEvent();

    time_t getTimeout() const;
    ErrorCode setTimeout(time_t timeout);
private:
    time_t timeout;
};

class HandleEvent: public Event {
public:
    enum Operation{
        OP_READ,
        OP_WRITE,
        OP_ERROR,
    };
    HandleEvent(HandleCb *cb, int handle, Operation op);
    ~HandleEvent();

    int getHandle() const;
    ErrorCode setHandle(int handle);
    Operation getOperation() const;
    ErrorCode setOperation(Operation op);
private:
    int handle;
    Operation op;
};

class Callback {
public:
    virtual ErrorCode call(Event *evt) const = 0;
};

class SignalCb: public Callback {
public:
    virtual ErrorCode signal(SignalEvent *evt) const = 0;
    ErrorCode call(Event *evt) const;
};

class TimerCb: public Callback {
public:
    virtual ErrorCode timeout(TimerEvent *evt) const = 0;
    ErrorCode call(Event *evt) const;
};

class HandleCb: public Callback {
public:
    virtual ErrorCode read(HandleEvent *evt) const = 0;
    virtual ErrorCode write(HandleEvent *evt) const = 0;
    virtual ErrorCode error(HandleEvent *evt) const = 0;
    ErrorCode call(Event *evt) const;
};

} // namespace event
