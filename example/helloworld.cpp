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
#include <event/loop.hpp>
#include <common/exception.hpp>
#include <common/error.hpp>
#include <common/log.hpp>
#include <platform/handle.hpp>

class MyTimerEvent: public event::TimerEvent {
 public:
    explicit MyTimerEvent(event::Loop *loop): count(0), loop(loop) {}

    u32 count;
    event::Loop *loop;
};

class MyTimerCb: public event::Callback<event::TimerEvent> {
 public:
    void onEvent(event::TimerEvent *e) const override {
        MyTimerEvent *mye = static_cast<MyTimerEvent *>(e);
        log_info("%s(): count: %u", __func__, mye->count++);
        mye->setTimeout(1000);
        mye->loop->TimerBus::addEvent(e, this);
    }
};

/// The main entry of the app
int app_main(int argc, char *argv[]) {
    try {
        event::Loop loop;
        MyTimerCb timerCb;
        MyTimerEvent timerEvent(&loop);

        timerEvent.setTimeout(1000);
        loop.TimerBus::addEvent(&timerEvent, &timerCb);

        common::Log::setLevel(common::Log::LOG_INFO);

        loop.start();
    } catch (platform::HandleException e) {
        log_err("Handle: %s, Message: %s", e.what(), e.message());
    } catch (common::Exception e) {
        log_err("%s, Message: %s", e.what(), e.message());
    }

    return 0;
}
