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

#include <event/callback.hpp>
#include <event/event.hpp>
#include <event/base.hpp>
#include <common/object.hpp>
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

class TimerEvent: public Event {
 public:
    explicit TimerEvent(common::Object *arg = nullptr): Event(arg), timeMs(0) {}

    virtual ~TimerEvent() {}

    void setTimeout(u32 ms);

    u64 getTimeMs() const;

 private:
    u64 timeMs;
};

typedef common::ObjectException<TimerEvent> TimerEventException;

class TimerBase: public Base<TimerEvent> {
 public:
    TimerBase(): timerHead(nullptr) {}

    ~TimerBase() override;

    void addEvent(TimerEvent *e, const Callback<TimerEvent> &cb) override;

    void delEvent(TimerEvent *e) override;

    int dispatch(int ms) override;

 private:
    class TimerNode;
    int timerAdvance();
    TimerNode *timerHead;
    platform::Lock mutex;
};

}  // namespace event
