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
#include <REvent/base.h>
#include <REvent/event.h>

using namespace std;
using namespace REvent;

class SignalHandle: public Handle {
    ErrorCode todo(Event *event) const {
        cout << "signal()" << endl;
        return RE_ERR_ERR;
    }

    ErrorCode error(Event *event) const {
        return RE_ERR_ERR;
    }

    ErrorCode timeout(Event *event) const {
        return RE_ERR_ERR;
    }
};

int main(int argc, char *argv[])
{
    Base base;
    SignalHandle handle;
    Event signalEvent(Event::EV_TYPE_SIGNAL, &handle);
    base.add(&signalEvent);
    base.dispatch();
    return 0;
}
