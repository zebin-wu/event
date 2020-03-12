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

/**
 * @file timer_event.hpp
 * @brief Timer event interfaces
*/

#define TIMER_DELAY_MAX 0x7fffffffU

namespace event {

class TimerEvent: public Event {
 public:
    explicit TimerEvent(common::Object *arg = nullptr): Event(arg), timeMs(0) {}

    /**
     * @brief Empty virtual destructor
    */
    virtual ~TimerEvent() {}

    void setTimeout(u32 ms);

    u64 getTimeMs() const;

 private:
    u64 timeMs;
};

typedef common::ObjectException<TimerEvent> TimerEventException;

class TimerBus: public Bus<TimerEvent> {
 public:
    TimerBus(): timerHead(nullptr) {}

    /**
	 * @brief Empty virtual destructor
	*/
    ~TimerBus() override;

    void addEvent(TimerEvent *e, const Callback<TimerEvent> *cb) override;

    void delEvent(TimerEvent *e) override;

    int dispatch() override;

 private:
    class TimerNode;
    int timerAdvance();
    TimerNode *timerHead;
    platform::Lock mutex;
};

}  // namespace event
