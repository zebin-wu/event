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

#include <common/object.hpp>
#include <common/exception.hpp>
#include <platform/type.hpp>
#include <platform/handle.hpp>
#include <platform/lock.hpp>

/**
 * @file event.hpp
 * @brief Class Event
*/

namespace event {

/**
 * @brief The base event class, all events inherit from this class
 */
class Event {
 public:
	/**
	 * @brief Default constructor
	*/
    Event(): pending(false) {}

    /**
     * @brief Constructor with the argument of event
     * 
     * @arg is the argument of event
    */
    explicit Event(common::Object *arg): pending(false), arg(arg) {}


	/**
	 * @brief Empty virtual destructor
	*/
    virtual ~Event() {}


    /**
     * @brief Whether the event has been canceled
     *
     * @return true if the event is pending
    */
    bool isPending() const {
        return pending;
    }


    /**
     * @brief Sets the pending status for the event
     * 
     * @param pending Whether the event is pending or not
    */
    void setPending(bool pending) {
        mutex.lock();
        pending = pending;
        mutex.unlock();
    }

    common::Object *getArg() const {
        return arg;
    }

    void setArg(common::Object *arg) {
        this->arg = arg;
    }

 private:
    bool pending;
    platform::Lock mutex;
    common::Object *arg;
};

}  // namespace event
