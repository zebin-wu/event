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
#include <common/exception.hpp>
#include <common/error.hpp>
#include <common/log.hpp>
#include <platform/handle.hpp>

/// The path of file to test the handle event
#define HELLOWORLD_FILE_PATH "/tmp/helloworld"

static event::Base base;

class MyTimerCb: public event::TimerCb {
 public:
    void timeout(event::TimerEvent *evt) const;
};

class MyTimerEvent: public event::TimerEvent {
 public:
    explicit MyTimerEvent(MyTimerCb *cb): count(0),
        TimerEvent(dynamic_cast<event::TimerCb *>(cb)) {}

    u32 count;
};

void MyTimerCb::timeout(event::TimerEvent *evt) const {
    MyTimerEvent *myEvt = static_cast<MyTimerEvent *>(evt);

    log_info("%s(): count: %u", __func__, myEvt->count++);
    evt->setTimeout(1000);
    base.addEvent(evt);
}

class MyHandleCb: public event::HandleCb {
 public:
    void write(event::HandleEvent *evt) const {
        platform::Handle *handle = evt->getHandle();
        log_info("writing...");
        handle->write("hello world.", sizeof("Hello world.\n"));
        base.delEvent(evt);
        evt->setOperation(event::HandleEvent::OP_READ);
        base.addEvent(evt);
    }
    void read(event::HandleEvent *evt) const {
        platform::Handle *handle = evt->getHandle();
        char buf[128] = "";
        handle->seek(platform::Handle::SEEK_MO_SET, 0);
        handle->read(static_cast<void *>(buf), sizeof(buf));
        log_info("%s", buf);
        base.delEvent(evt);
    }
};

/// The main entry of the app
int app_main(int argc, char *argv[]) {
    try {
        MyTimerCb timerCb;
        MyTimerEvent timerEvent(&timerCb);

        timerEvent.setTimeout(1000);
        base.addEvent(&timerEvent);

        MyHandleCb handleCb;
        platform::Handle handle(HELLOWORLD_FILE_PATH,
            platform::Handle::MO_CREAT |
            platform::Handle::MO_WRITE |
            platform::Handle::MO_READ);
        event::HandleEvent
            handleEvent(&handleCb, &handle, event::HandleEvent::OP_WRITE);
        base.addEvent(&handleEvent);

        common::Log::setLevel(common::Log::LOG_INFO);

        base.dispatch();
    } catch (platform::HandleException e) {
        log_err("Handle: %s, Message: %s", e.what(), e.message());
    } catch (common::Exception e) {
        log_err("%s, Message: %s", e.what(), e.message());
    }

    return 0;
}
