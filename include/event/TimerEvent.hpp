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

#include <event/Callback.hpp>
#include <event/Event.hpp>
#include <event/Base.hpp>
#include <common/exception.hpp>
#include <common/assert.hpp>
#include <common/log.hpp>
#include <platform/lock.hpp>
#include <platform/clock.hpp>

/**
 * @file TimerEvent.hpp
 * @brief Event base class
*/

#define TIMER_DELAY_MAX 0x7fffffffU

namespace event {

typedef common::ClassException<TimerEvent> TimerEventException;

class TimerEvent: public Event {
 public:
    TimerEvent(): timeMs(0) {}
    virtual ~TimerEvent() {}

    void setTimeout(u32 ms) {
        if (this->isPending()) {
            throw TimerEventException(this, common::ERR_PERM,
                "the event has been added, cannot set timeout");
        }
        if (ms > TIMER_DELAY_MAX) {
            ms = TIMER_DELAY_MAX;
        }
        timeMs = platform::Clock::Instance().getTotalMs() + ms;
        if (!timeMs) {
            timeMs--;
        }
    }

    u64 getTimeMs() const {
        return timeMs;
    }

 private:
    u64 timeMs;
};

class TimerBase: public Base<TimerEvent> {
 public:
    TimerBase(): timerHead(nullptr) {}

    ~TimerBase() override {
        TimerNode *cur;
        while (timerHead) {
            cur = timerHead;
            timerHead = cur->next;
            delete cur;
        }
    }

    void addEvent(TimerEvent & e, Callback<TimerEvent> & cb) override {
        u64 timeMs;
        TimerNode **t = &timerHead;
        timeMs = e.getTimeMs();
        if (e.isPending()) {
            throw TimerEventException(e, common::ERR_BUSY, "the event was added");
            return;
        }
        e.setPending(true);
        mutex.lock();
        while (*t) {
            if (TIME_AFTER((*t)->e.getTimeMs(), timeMs)) {
                break;
            }
            t = &(*t)->next;
        }
        *t = new TimerNode(e, cb, *t);
        mutex.unlock();
    }

    void delEvent(TimerEvent & e) override {
        TimerNode **t = &timerHead, *cur;

        if (!e.isPending()) {
            throw TimerEventException(e, common::ERR_BUSY, "the event was not added");
            return;
        }
        e.setPending(false);
        mutex.lock();
        while (*t) {
            cur = *t;
            if (&(cur->e) == &e) {
                *t = cur->next;
                mutex.unlock();
                delete cur;
                return;
            }
        }
        mutex.unlock();
        throw TimerEventException(e, common::ERR_PERM, "the event was not found");
    }

    int dispatch(int ms) override {
        return timerAdvance() + ms;
    }

    int timerAdvance() {
        TimerEvent * curEvt;
        TimerCb * curCb;
        TimerNode *cur;
        u64 curMs, tmpMs;

        while (timerHead) {
            mutex.lock();
            cur = timerHead;
            curEvt = &(cur->e);
            curCb = &(cur->cb);
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
            curCb->onEvent(*curEvt);
        }
        return -1;
    }

 private:
    typedef Callback<TimerEvent> TimerCb;
    class TimerNode {
     public:
        explicit TimerNode(TimerEvent & e, TimerCb & cb, TimerNode *next = nullptr):
            e(e), next(next), cb(cb) {}

        TimerEvent & e;
        TimerCb & cb;
        TimerNode *next;
    } *timerHead;
    platform::Lock mutex;
};

}  // namespace event
