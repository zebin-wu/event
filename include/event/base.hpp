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

#include <type_traits>
#include <event/callback.hpp>
#include <common/exception.hpp>

/**
 * @file base.hpp
 * @brief Event base class
*/

namespace event {

// Forward declare the Event class
class Event;

template <class T>
class Base {
 public:
    Base() {
        // An error here indicates you're trying to implement
        // EventHandler with a type that is not derived from Event
        static_assert(std::is_base_of<Event, T>::value,
            "Base<T>: T must be a class derived from Event");
    }
    virtual ~Base() {}
    virtual void addEvent(T *e, const Callback<T> &cb) = 0;
    virtual void delEvent(T *e) = 0;
    virtual int dispatch(int ms) = 0;
};

}  // namespace event
