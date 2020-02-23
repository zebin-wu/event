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

#include <common/exception.hpp>
#include <platform/type.hpp>
#include <platform/handle.hpp>

/**
 * @file event.hpp
 * @brief Event class
*/

namespace event {

class EventPriv;
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

    explicit Event(Type type, Callback *cb);
    ~Event();

    Type getType() const;
    Callback *getCb() const;
    void setCb(Callback *cb);
    bool isPending() const;
    void setPending(bool pending);
 private:
    EventPriv *priv;
};

class EventException: public common::Exception {
 public:
    explicit EventException(Event *evt, common::ErrorCode err):
        Exception(err), evt(evt) {}
    explicit EventException(Event *evt,
        common::ErrorCode err, const char *message):
        Exception(err, message), evt(evt) {}
 private:
    Event *evt;
};

class SignalEvent: public Event {
 public:
    enum Signal {
        SIGNAL_INT,
    };
    explicit SignalEvent(SignalCb *cb, Signal signal);
    ~SignalEvent();

    Signal getSignal() const;
    void setSignal(Signal signal);
 private:
    Signal signal;
};

class TimerEvent: public Event {
 public:
    explicit TimerEvent(TimerCb *cb);
    ~TimerEvent();

    void setTimeout(u32 ms);
    u64 getTimeMs() const;
 private:
    u64 timeMs;
};

class HandleEvent: public Event {
 public:
    enum Operation{
        OP_READ,        ///< read
        OP_WRITE,       ///< write
    };
    explicit HandleEvent(HandleCb *cb, platform::Handle *handle, Operation op);
    ~HandleEvent();

    platform::Handle *getHandle() const;
    void setHandle(platform::Handle *handle);
    Operation getOperation() const;
    void setOperation(Operation op);
 private:
    platform::Handle *handle;
    Operation op;
};

class Callback {
 public:
    virtual void call(Event *evt) const = 0;
};

class SignalCb: public Callback {
 public:
    virtual void signal(SignalEvent *evt) const = 0;
    void call(Event *evt) const;
};

class TimerCb: public Callback {
 public:
    virtual void timeout(TimerEvent *evt) const = 0;
    void call(Event *evt) const;
};

class HandleCb: public Callback {
 public:
    virtual void read(HandleEvent *evt) const = 0;
    virtual void write(HandleEvent *evt) const = 0;
    virtual void exception(HandleEvent *evt) const = 0;
    void call(Event *evt) const;
};

}  // namespace event
