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
# Makefile of Event
#

QUIET ?= @

ARCH ?= native
VERSION ?= 0.9

#
# Directories
#
ROOT_DIR = .
BUILD_DIR = $(ROOT_DIR)/build/$(ARCH)

#
# Toolchain
#
CPP := $(CROSS_COMPILE)g++
LD := $(CROSS_COMPILE)ld

#
# Link a target
#
define METHOD_LD
	echo 'LINK $(notdir $1)';\
	    $(CPP) -o $1 $(2:%.cpp=$(BUILD_DIR)/%.o) $3 $(LDFLAGS)

-include $(2:%.cpp=$(BUILD_DIR)/%.d)
endef

#
# Make a static library.
#
define METHOD_AR
	echo 'AR $(notdir $1)';\
	    $(AR) cr $1 $(2:%.cpp=$(BUILD_DIR)/%.o)
endef

#
# Build a target
#
# $1: Target name
# $2: Method
# $3: Source files
# $4: Extra link switchs
#
define BUILD_TARGET_RULES
$1: $(3:%.cpp=$(BUILD_DIR)/%.o)
	$(QUIET)[ -d $(shell dirname $1) ] || mkdir -p $(shell dirname $1);\
	    $(call $2, $1, $3, $4)
endef

#
# Defines of library
#
LIB_NAME = libevt
LIB_DIR = $(BUILD_DIR)/lib
LIB_DYNAMIC = $(LIB_DIR)/$(LIB_NAME).so.$(VERSION)
LIB_STATIC = $(LIB_DIR)/$(LIB_NAME).a

#
# Header files include directory
#
INCLUDES := \
	include\
	$(NULL)

#
# Source file of libevt
#
SOURCES_LIBEVT := \
	$(wildcard src/event/*.cpp)\
	$(NULL)

#
# Source file of example
#
SOURCES_EXAMPLE := \
	$(wildcard example/*.cpp)\
	$(NULL)

#
# Compile command line switch of CPP
#
CPPFLAGS += \
	$(addprefix -I,$(INCLUDES))

#
# Compile command line switch of LD
#
LDFLAGS += -fPIC

#
# Rule to build all.
#
.PHONY: all
all: $(LIB_DYNAMIC) $(LIB_STATIC)

#
# Rule to build $(LIB_NAME).so.
#
$(eval $(call BUILD_TARGET_RULES, $(LIB_DYNAMIC), METHOD_LD,\
	$(SOURCES_LIBEVT),\
	-shared))

#
# Rule to build $(LIB_NAME).a.
#
$(eval $(call BUILD_TARGET_RULES, $(LIB_STATIC), METHOD_AR,\
	$(SOURCES_LIBEVT)))

#
# Rule to compile source code
#
$(BUILD_DIR)/%.o: %.cpp
	$(QUIET)echo 'CPP $<';\
	    mkdir -p $(dir $@); $(CPP) $(CPPFLAGS) -c -o $@ $<

#
# Rule to make dependencies files
#
$(BUILD_DIR)/%.d: %.cpp Makefile
	$(QUIET)echo "DEP $<";\
		(mkdir -p $(dir $@); $(CPP) -MM $(CPPFLAGS) $< | \
		sed 's,.*\.o[ :]*,$(@:%.d=%.o) $@: ,g' > $@) || rm -f $@

#
# Clean staging directory.
#
.PHONY: clean
clean:
	$(QUIET)echo 'Clean staging directory'; rm -rf $(BUILD_DIR)
