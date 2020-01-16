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
# Makefile of REvent
#
PROJECT_NAME := revent

QUIET ?= @

ARCH ?= native
VERSION ?= 0.9

#
# Directories
#
ROOT_DIR = .
BUILD_DIR = $(ROOT_DIR)/build/$(ARCH)
BIN_DIR = $(BUILD_DIR)/bin
LIB_DIR = $(BUILD_DIR)/lib

#
# Toolchain
#
CXX ?= $(CROSS_COMPILE)g++
AR ?= $(CROSS_COMPILE)ar

#
# Link a target
#
# $1: Target name
# $2: Source files
# $3: Extra link switchs
#
define METHOD_LD
	echo 'LINK $(notdir $1)';\
	    $(CXX) -o $1 $(2:%.cpp=$(BUILD_DIR)/%.o) $3 $(LDFLAGS)

-include $(2:%.cpp=$(BUILD_DIR)/%.d)
endef

#
# Make a static library
#
# $1: Target name
# $2: Source files
#
define METHOD_AR
	echo 'AR $(notdir $1)';\
	    $(AR) cr $1 $(2:%.cpp=$(BUILD_DIR)/%.o)
endef

#
# Rule to build a target
#
# $1: Target name
# $2: Method
# $3: Source files
# $4: Extra link switchs
# $5: Dependent target
#
define BUILD_TARGET_RULES
$1: $(3:%.cpp=$(BUILD_DIR)/%.o) $5
	$(QUIET)[ -d $(shell dirname $1) ] || mkdir -p $(shell dirname $1);\
	    $(call $2, $1, $3, $4)
endef

#
# Defines of library
#
LIB_NAME = lib$(PROJECT_NAME)
LIB_DYNAMIC = $(LIB_DIR)/$(LIB_NAME).so.$(VERSION)
LIB_STATIC = $(LIB_DIR)/$(LIB_NAME).a

#
# Header files include directory
#
INCLUDES := \
	include\
	$(NULL)

#
# Source file of librevent
#
SOURCES_LIBREVENT := \
	$(wildcard src/$(PROJECT_NAME)/*.cpp)\
	$(NULL)

#
# Compile command line switch of CPP
#
CPPFLAGS += \
	$(addprefix -I, $(INCLUDES))\
	-fPIC -ffunction-sections -fdata-sections

#
# Compile command line switch of LD
#
LDFLAGS += -fPIC -Wl,--gc-sections

#
# Rule to build all
#
.PHONY: all
all: $(LIB_DYNAMIC) $(LIB_STATIC)

#
# Rule to build example
#
EXAMPLE_LDFLAGS =\
	-L$(LIB_DIR) -l$(PROJECT_NAME)
EXAMPLE_TARGET = \
	helloworld\
	$(NULL)

.PHONY: example
example: $(addprefix $(BIN_DIR)/, $(EXAMPLE_TARGET))

#
# Rule to build dynamic library
#
$(eval $(call BUILD_TARGET_RULES, $(LIB_DYNAMIC), METHOD_LD,\
	$(SOURCES_LIBREVENT),\
	-shared))

#
# Rule to build static library
#
$(eval $(call BUILD_TARGET_RULES, $(LIB_STATIC), METHOD_AR,\
	$(SOURCES_LIBREVENT)))

#
# Rule to build helloworld
#
$(eval $(call BUILD_TARGET_RULES, $(BIN_DIR)/helloworld, METHOD_LD,\
	example/helloworld.cpp, $(EXAMPLE_LDFLAGS), all))

#
# Rule to compile source code
#
$(BUILD_DIR)/%.o: %.cpp
	$(QUIET)echo 'CPP $<';\
	    mkdir -p $(dir $@); $(CXX) $(CPPFLAGS) -c -o $@ $<

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
