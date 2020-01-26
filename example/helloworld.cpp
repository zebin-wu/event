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
#include <iostream>
#include <event/base.hpp>
#include <event/event.hpp>
#include <common/error.hpp>
#include <common/log.hpp>

static event::Base base;

class MyTimerCb: public event::TimerCb {
 public:
    ErrorCode timeout(event::TimerEvent *evt) const;
};

class MyTimerEvent: public event::TimerEvent {
 public:
    explicit MyTimerEvent(MyTimerCb *cb): count(0),
        TimerEvent(dynamic_cast<event::TimerCb *>(cb)) {}

    u32 count;
};

ErrorCode MyTimerCb::timeout(event::TimerEvent *evt) const {
    MyTimerEvent *myEvt = static_cast<MyTimerEvent *>(evt);

    log_info("%s(): count: %u", __func__, myEvt->count++);
    evt->setTimeout(1000);
    base.addEvent(evt);
    return common::ERR_ERR;
}

int main(int argc, char *argv[]) {
    MyTimerCb timerCb;
    MyTimerEvent timerEvent(&timerCb);
    timerEvent.setTimeout(1000);
    base.addEvent(&timerEvent);

    common::Log::setLevel(common::Log::LOG_INFO);

    base.dispatch();

    return 0;
}
