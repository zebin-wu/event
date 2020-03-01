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
#include <event/base.hpp>
#include <event/event.hpp>
#include <common/assert.hpp>
#include <common/log.hpp>
#include <platform/lock.hpp>
#include <platform/clock.hpp>
#include <platform/poll.hpp>

namespace event {

class TimerBase {
 public:
    TimerBase(): timerHead(nullptr) {}

    ~TimerBase() {
        TimerNode *cur;
        while (timerHead) {
            cur = timerHead;
            timerHead = cur->next;
            delete cur;
        }
    }

    void addEvent(TimerEvent *evt) {
        u64 timeMs;
        TimerNode **t = &timerHead;
        timeMs = evt->getTimeMs();
        mutex.lock();
        while (*t) {
            if (TIME_AFTER((*t)->evt->getTimeMs(), timeMs)) {
                break;
            }
            t = &(*t)->next;
        }
        *t = new TimerNode(evt, *t);
        mutex.unlock();
    }

    void delEvent(TimerEvent *evt) {
        TimerNode **t = &timerHead, *cur;

        mutex.lock();
        while (*t) {
            cur = *t;
            if (cur->evt == evt) {
                *t = cur->next;
                mutex.unlock();
                delete cur;
                return;
            }
        }
        mutex.unlock();
        throw EventException(evt, common::ERR_PERM, "the event was not found");
    }

    int timerAdvance() {
        TimerEvent *curEvt;
        TimerNode *cur;
        u64 curMs, tmpMs;

        while (timerHead) {
            mutex.lock();
            cur = timerHead;
            curEvt = cur->evt;
            mutex.unlock();
            tmpMs = curEvt->getTimeMs();
            curMs = platform::Clock::Instance().getTotalMs();
            if (TIME_AFTER(tmpMs, curMs)) {
                return static_cast<int>(tmpMs - curMs);
            }
            mutex.lock();
            timerHead = cur->next;
            mutex.unlock();
            delete cur;
            curEvt->setPending(false);
            (curEvt->getCb())->call(static_cast<Event *>(curEvt));
        }
        return -1;
    }

 private:
    class TimerNode {
     public:
        explicit TimerNode(TimerEvent *evt, TimerNode *next = nullptr):
            evt(evt), next(next) {}

        TimerEvent *evt;
        TimerNode *next;
    } *timerHead;
    platform::Lock mutex;
};

class HandleBase {
 public:
    void addEvent(HandleEvent *evt) {
        poll.add(evt->getHandle(), getPollMode(evt->getOperation()),
            [] (platform::Poll::PollMode mode,
                platform::Handle *handle, void *arg) {
                HandleEvent *evt = static_cast<HandleEvent *>(arg);
                evt->getCb()->call(evt);
            }, evt);
    }

    void delEvent(HandleEvent *evt) {
        poll.del(evt->getHandle(), getPollMode(evt->getOperation()));
    }

    void wait(int ms) {
        poll.polling(ms);
    }

 private:
    platform::Poll::PollMode getPollMode(HandleEvent::Operation op) {
        switch (op) {
        case HandleEvent::OP_READ:
            return platform::Poll::POLL_READ;
            break;
        case HandleEvent::OP_WRITE:
            return platform::Poll::POLL_WRITE;
            break;
        case HandleEvent::OP_EXCEPTION:
            return platform::Poll::POLL_ERR;
        }
    }
    platform::Poll poll;
};

class BasePriv {
 public:
    BasePriv(): loop(false) {}

    TimerBase timerBase;
    HandleBase handleBase;
    bool loop;
};

Base::Base(): priv(new BasePriv) {}

Base::~Base() {
    delete priv;
}

void Base::addEvent(Event *evt) {
    ASSERT(evt);
    if (evt->isPending()) {
        throw EventException(evt, common::ERR_BUSY, "the event has been added");
    }
    switch (evt->getType()) {
    case Event::EV_TIMER:
        priv->timerBase.addEvent(static_cast<TimerEvent *>(evt));
        break;
    case Event::EV_HANDLE:
        priv->handleBase.addEvent(static_cast<HandleEvent *>(evt));
        break;
    default:
        ASSERT_NOTREACHED();
    }
    evt->setPending(true);
}

void Base::delEvent(Event *evt) {
    ASSERT(evt);
    if (!evt->isPending()) {
        throw EventException(evt, common::ERR_BUSY, "the event is not added");
    }
    switch (evt->getType()) {
    case Event::EV_TIMER:
        priv->timerBase.delEvent(static_cast<TimerEvent *>(evt));
    case Event::EV_HANDLE:
        priv->handleBase.delEvent(static_cast<HandleEvent *>(evt));
        break;
    default:
        ASSERT_NOTREACHED();
    }
    evt->setPending(false);
}

void Base::dispatch() {
    int timeout;

    if (priv->loop) {
        throw BaseException(this, common::ERR_BUSY, "the base is in loop");
    }
    priv->loop = true;

    while (priv->loop) {
        timeout = priv->timerBase.timerAdvance();
        priv->handleBase.wait(timeout);
    }
}

}  // namespace event
