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
#include <common/error.hpp>
#include <common/assert.hpp>
#include <common/log.hpp>
#include <platform/lock.hpp>
#include <platform/clock.hpp>

namespace event {

class TimerBase {
 public:
    TimerBase(): timerHead(NULL) {}

    ErrorCode addEvent(TimerEvent *evt) {
        u64 timeMs;
        TimerNode **t = &timerHead;
        timeMs = evt->getTimeMs();
        mutex.lock();
        while (*t != NULL) {
            if (TIME_AFTER((*t)->evt->getTimeMs(), timeMs)) {
                break;
            }
            t = &(*t)->next;
        }
        *t = new TimerNode(evt, *t);
        mutex.unlock();
        return common::ERR_OK;
    }

    ErrorCode delEvent(TimerEvent *evt) {
        TimerNode **t = &timerHead, *cur;

        mutex.lock();
        while (*t != NULL) {
            cur = *t;
            if (cur->evt == evt) {
                *t = cur->next;
                delete cur;
                return common::ERR_OK;
            }
        }
        mutex.unlock();
        return common::ERR_INVAL_ARG;
    }

    u32 timerAdvance() {
        TimerEvent *curEvt;
        TimerNode **t = &timerHead, *cur;
        u64 curMs, tmpMs;

        while (*t != NULL) {
            mutex.lock();
            cur = *t;
            curEvt = cur->evt;
            mutex.unlock();
            tmpMs = curEvt->getTimeMs();
            curMs = platform::Clock::Instance().getTotalMs();
            if (TIME_AFTER(tmpMs, curMs)) {
                return tmpMs - curMs;
            }
            mutex.lock();
            *t = cur->next;
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
        explicit TimerNode(TimerEvent *evt, TimerNode *next = NULL):
            evt(evt), next(next) {}

        TimerEvent *evt;
        TimerNode *next;
    } *timerHead;
    platform::Lock mutex;
};

class BasePriv {
 public:
    BasePriv(): loop(false) {}

    TimerBase timerBase;
    bool loop;
};

Base::Base(): priv(new BasePriv) {}

Base::~Base() {
    delete priv;
}

ErrorCode Base::addEvent(Event *evt) {
    ErrorCode err = common::ERR_OK;

    ASSERT(evt);
    if (evt->isPending()) {
        return common::ERR_BUSY;
    }
    switch (evt->getType()) {
    case Event::EV_TIMER:
        priv->timerBase.addEvent(static_cast<TimerEvent *>(evt));
        break;
    default:
        err = common::ERR_INVAL_ARG;
        goto end;
    }
    evt->setPending(true);
end:
    return err;
}

ErrorCode Base::delEvent(Event *evt) {
    ErrorCode err = common::ERR_OK;

    ASSERT(evt);
    if (!evt->isPending()) {
        return common::ERR_IDLE;
    }
    switch (evt->getType()) {
    case Event::EV_TIMER:
        priv->timerBase.delEvent(static_cast<TimerEvent *>(evt));
    default:
        err = common::ERR_INVAL_ARG;
        goto end;
    }
    evt->setPending(false);
end:
    return err;
}

ErrorCode Base::dispatch() {
    priv->loop = true;
    u32 ms;

    while (priv->loop) {
        ms = priv->timerBase.timerAdvance();
    }
    return common::ERR_OK;
}

}  // namespace event
