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

#include <event/error.h>

namespace event {

class Handle;

class Event {
public:
    enum Type {
        EV_TYPE_TIMEOUT,
        EV_TYPE_READ,
        EV_TYPE_WRITE,
        EV_TYPE_SIGNAL,
        EV_TYPE_MAX,
    };

    Event(Type type, Handle *handle);
    ~Event();

    Type getType() const;
    ErrorCode setType(Type type);
private:
    Type type;
    Handle *handle;
};

class Handle {
public:
    virtual ErrorCode todo(Event *event) const = 0;
    virtual ErrorCode error(Event *event) const = 0;
    virtual ErrorCode timeout(Event *event) const = 0;
};

}
