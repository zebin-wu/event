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
```
#include <common/event.hpp>
#include <common/base.hpp>
```

## License

[MIT © 2020 KNpTrue.](LICENSE)