# Event

![](https://img.shields.io/badge/language-c++-orange.svg)
![](https://img.shields.io/badge/platform-linux-lightgrey.svg)
[![license](https://img.shields.io/github/license/KNpTrue/event)](LICENSE)

## Contents

- [Introduction](#introduction)
- [Code style](#Code-style)
- [Build](#Build)
- [Usage](#usage)
- [License](#license)

## Introduction

Event is an event notification library implemented in C++11 that provides IO, timer and signal events, all events will be triggered asynchronously. At first it only supported the linux platform, and later it will implement more platforms in the submodule [common](https://github.com/KNpTrue/common).

## Code style

The library supports code style checking, the checker is [cpplint](https://github.com/google/styleguide), a command line tool that checks for style issues in C/C++ files according to the [Google C++ Style Guide](http://google.github.io/styleguide/cppguide.html).

## Build
Before building, you need to make sure that cpplint is installed on the system, if not, you can use pip to install.
```
pip install cpplint
```
Then you can build the library.
```
make
```
You can build examle with the following command, these examples will be placed under `./build/{platform}/bin`，the premise of executing them is to link the library, see [Install](#Install).
```
make example
```
## Install
Install the library to your system or the specified path(Set by the environment variable `INSTALL_DIR`)，as shown in the following command, the library will be installed under `/lib`.
```
make INSTALL_DIR=/ install
```

## Usage

Note: See [helloworld.cpp](example/helloworld.cpp) to get more information.

1. Contains the necessary header files and use namespace event
```
#include <event/event.hpp>
#include <event/bus.hpp>
#include <event/callback.hpp>

#include <queue>
#include <iostream>
#include <platform/lock.hpp>
```
2. Implement your own event, but need to inherit from Event
```
class MyEvent: public event::Event {
 public:
    MyEvent(const char *msg = nullptr): msg(msg) {}

    const char *getMsg() const {
        return msg;
    }

 private:
    const char *msg;
};
```
3. Implement your own event bus, but need to inherit from Bus
```
class MyBus: public event::Bus<MyEvent> {
 public:
    void addEvent(MyEvent *e, const event::Callback<MyEvent> *cb) override {
        mutex.lock();
        events.push(new EventState(e, cb));
        mutex.unlock();
    }

    void delEvent(MyEvent *e) {}


    int dispatch() {
        mutex.lock();
        EventState *s = events.front();
        events.pop();
        mutex.unlock();
        s->cb->onEvent(s->e);
        delete s;

        return -1;
    }
 private:
    class EventState {
     public:
        EventState(MyEvent *e, const event::Callback<MyEvent> *cb):
            e(e), cb(cb) {}
        MyEvent *e;
        const event::Callback<MyEvent> *cb;
    };
    std::queue<EventState *> events;
    platform::Lock mutex;
};
```
4. Implement the callback of event
```
class MyEventCb: public event::Callback<MyEvent> {
 public:
    void onEvent(MyEvent *e) const override {
        std::cout << "msg: " << e->getMsg() << std::endl;
    }
};
```
5. Trigger event
```
    MyEvent e("hello world");
    MyBus bus;
    MyEventCb cb;
    bus.addEvent(&e, &cb);
    bus.dispatch();
```

## License

[MIT © 2020 KNpTrue.](LICENSE)