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
#include <event/event.hpp>
#include <common/assert.hpp>
#include <platform/lock.hpp>
#include <platform/clock.hpp>

#define TIMER_DELAY_MAX 0x7fffffffU

namespace event {

class EventPriv {
 public:
    EventPriv(Event::Type type, Callback *cb):
        type(type), cb(cb), pending(false) {}

    Event::Type type;
    Callback *cb;
    bool pending;
    platform::Lock mutex;
};

Event::Event(Type type, Callback *cb): priv(new EventPriv(type, cb)) {}

Event::~Event() {}

Event::Type Event::getType() const {
    return priv->type;
}

Callback *Event::getCb() const {
    return priv->cb;
}

ErrorCode Event::setCb(Callback *cb) {
    ASSERT(cb);
    if (isPending()) {
        return common::ERR_BUSY;
    }
    priv->cb = cb;
    return common::ERR_OK;
}

bool Event::isPending() const {
    return priv->pending;
}

void Event::setPending(bool pending) {
    priv->mutex.lock();
    priv->pending = pending;
    priv->mutex.unlock();
}

SignalEvent::SignalEvent(SignalCb *cb, Signal signal):
    Event(Event::EV_SIGNAL, static_cast<Callback *>(cb)), signal(signal) {}

SignalEvent::~SignalEvent() {}

SignalEvent::Signal SignalEvent::getSignal() const {
    return this->signal;
}

ErrorCode SignalEvent::setSignal(Signal signal) {
    this->signal = signal;
    return common::ERR_OK;
}

TimerEvent::TimerEvent(TimerCb *cb):
    Event(Event::EV_TIMER, static_cast<Callback *>(cb)), timeMs(0) {}

TimerEvent::~TimerEvent() {}

ErrorCode TimerEvent::setTimeout(u32 ms) {
    if (this->isPending()) {
        return common::ERR_BUSY;
    }
    if (ms > TIMER_DELAY_MAX) {
        ms = TIMER_DELAY_MAX;
    }
    timeMs = platform::Clock::Instance().getTotalMs() + ms;
    if (!timeMs) {
        timeMs--;
    }

    return common::ERR_OK;
}

u64 TimerEvent::getTimeMs() const {
    return timeMs;
}

HandleEvent::HandleEvent(HandleCb *cb, int handle, Operation op):
    Event(Event::EV_HANDLE, static_cast<Callback *>(cb)),
    handle(handle), op(op) {}

HandleEvent::~HandleEvent() {}

int HandleEvent::getHandle() const {
    return handle;
}

ErrorCode HandleEvent::setHandle(int handle) {
    this->handle = handle;
    return common::ERR_OK;
}

HandleEvent::Operation HandleEvent::getOperation() const {
    return this->op;
}

ErrorCode HandleEvent::setOperation(Operation op) {
    this->op = op;
    return common::ERR_OK;
}

ErrorCode SignalCb::call(Event *evt) const {
    ASSERT(evt->getType() == Event::EV_SIGNAL);
    return signal(static_cast<SignalEvent *>(evt));
}

ErrorCode TimerCb::call(Event *evt) const {
    ASSERT(evt->getType() == Event::EV_TIMER);
    return timeout(static_cast<TimerEvent *>(evt));
}

ErrorCode HandleCb::call(Event *evt) const {
    HandleEvent *handleEvt = static_cast<HandleEvent *>(evt);
    ASSERT(evt->getType() == Event::EV_HANDLE);

    switch (handleEvt->getOperation()) {
    case HandleEvent::OP_READ:
        return read(handleEvt);
    case HandleEvent::OP_WRITE:
        return write(handleEvt);
    case HandleEvent::OP_ERROR:
        return error(handleEvt);
    default:
        ASSERT(true);
        break;
    }
    return common::ERR_ERR;
}

}  // namespace event
