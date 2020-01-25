#
# Copyright (c) 2020 KNpTrue
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# Makefile of event
#
PROJECT_NAME := event

QUIET ?= @

PLATFORM ?= linux
ARCH ?= native
VERSION ?= 0.9

#
# Directories
#
ROOT_DIR = .
COMMON_DIR = $(ROOT_DIR)/common
BUILD_DIR = $(ROOT_DIR)/build/$(ARCH)
BIN_DIR = $(BUILD_DIR)/bin
LIB_DIR = $(BUILD_DIR)/lib

#
# Toolchain
#
CXX ?= $(CROSS_COMPILE)g++
AR ?= $(CROSS_COMPILE)ar

#
# The common definition
#
include $(COMMON_DIR)/make/common_defs.mk

#
# Defines of library event
#
LIBEVENT_NAME = lib$(PROJECT_NAME)
LIBEVENT_DYNAMIC = $(LIB_DIR)/$(LIBEVENT_NAME).so.$(VERSION)
LIBEVENT_STATIC = $(LIB_DIR)/$(LIBEVENT_NAME).a

#
# Header files include directory
#
INCLUDES += \
	include\
	$(NULL)

#
# Source file of libevent
#
SOURCES_LIBEVENT := \
	$(wildcard src/$(PROJECT_NAME)/*.cpp)\
	$(NULL)

#
# Source file of example
#
SOURCES_EXAMPLE := \
	$(wildcard example/*.cpp)\
	$(NULL)

#
# Source files of all
#
SOURCES += \
	$(SOURCES_LIBEVENT)\
	$(SOURCES_EXAMPLE)\
	$(NULL)

#
# Compile command line switch of CPP
#
CPPFLAGS += $(addprefix -I, $(INCLUDES))

#
# Rule to build all
#
.PHONY: all
all: common $(LIBEVENT_NAME)

#
# The common rules
#
include $(COMMON_DIR)/make/common_rules.mk

#
# Rule to build $(LIBEVENT)
#
.PHONY: $(LIBEVENT_NAME)
$(LIBEVENT_NAME): $(LIBEVENT_DYNAMIC) $(LIBEVENT_STATIC)

#
# Rule to build example
#
EXAMPLE_LDFLAGS =\
	-L$(LIB_DIR) -l$(PROJECT_NAME) -lcommon
EXAMPLE_TARGET = \
	helloworld\
	$(NULL)

.PHONY: example
example: all $(addprefix $(BIN_DIR)/, $(EXAMPLE_TARGET))

#
# Rule to build dynamic library
#
$(eval $(call BUILD_TARGET_RULES, $(LIBEVENT_DYNAMIC), METHOD_LD,\
	$(SOURCES_LIBEVENT),\
	-shared))

#
# Rule to build static library
#
$(eval $(call BUILD_TARGET_RULES, $(LIBEVENT_STATIC), METHOD_AR,\
	$(SOURCES_LIBEVENT)))

#
# Rule to build helloworld
#
$(eval $(call BUILD_TARGET_RULES, $(BIN_DIR)/helloworld, METHOD_LD,\
	example/helloworld.cpp, $(EXAMPLE_LDFLAGS)))
