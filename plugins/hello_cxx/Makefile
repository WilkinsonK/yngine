CC=g++
CFLAGS=-Wall -Wextra -Wpedantic -std=c++23 -O3

OS := $(shell uname)
AARCH := $(shell uname -m)
DLIB_ROOT := ../../targets
DLIB_INCLUDE := ../../plugin/include
DLIB_SUFFIX := .so

ifeq ($(OS), Linux)
	OS=linux
else ifeq ($(OS), Darwin)
	export MACOSX_DEPLOYMENT_TARGET=15.6
	DLIB_SUFFIX := .dylib
	OS=macosx
endif

LIB_NAME := libhello_cxx$(DLIB_SUFFIX)
DLIB_MODE := release
DLIB_NAME := plugin
DLIB_PATH := $(DLIB_ROOT)/$(OS)/$(AARCH)/$(DLIB_MODE)
DLIB_FILE := $(DLIB_PATH)/lib$(DLIB_NAME)$(DLIB_SUFFIX)
DLIB_FLAGS := -l$(DLIB_NAME) -L$(DLIB_PATH) -I$(DLIB_INCLUDE)

.PHONY: all
all: build

.PHONY: clean
clean:
	@rm $(LIB_NAME) $(LIB_NAME).o

.PHONY: build
build: $(LIB_NAME) $(DLIB_FILE)

$(LIB_NAME): $(LIB_NAME).o
	@$(CC) -shared -o $@ $^ $(CFLAGS) $(DLIB_FLAGS)

$(LIB_NAME).o: plugin.cpp
	@$(CC) -c -fPIC -o $@ $^ $(CFLAGS) -I$(DLIB_INCLUDE)
