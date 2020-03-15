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
 * @file bus.hpp
 * @brief Class Bus
*/

namespace event {

// Forward declare the Event class
class Event;

/**
 * @brief Base bus class, all bus inherit from this class
*/
template <class T>
class Bus {
 public:
    /**
     * @brief Default constructor that enforces the template type
    */
    Bus() {
        // An error here indicates you're trying to implement
        // event with a type that is not derived from Event
        static_assert(std::is_base_of<Event, T>::value,
            "Bus<T>: T must be a class derived from Event");
    }


    /**
     * @brief Empty virtual destructor
    */
    virtual ~Bus() {}


    /**
     * @brief Add the event to the bus
     * 
     * @param e is a pointer to the event
     * @param cb is the reference of the callback
    */
    virtual void addEvent(T *e, const Callback<T> *cb) = 0;


    /**
     * @brief Delete the event from the bus
     * 
     * @param e is a pointer to the event
    */
    virtual void delEvent(T *e) = 0;


    /**
     * @brief Call the callback corresponding to the event when the event is triggered
     * 
     * @return Specifies the maximum wait time to return @c dispatch(), -1 if it do not need to wait
    */
    virtual int dispatch() { return -1; }


    /**
     * @brief Call the callback corresponding to the event when the event is triggered
     * 
     * @param timeout Specifies the maximum wait time in milliseconds(-1 == infinite)
     * 
     * @return Specifies the maximum wait time to return @c dispatch(), -1 if it do not need to wait
    */
    virtual int dispatch(int timeout) { return timeout; }
};

}  // namespace event
