#
# Copyright (c) Flyover Games, LLC
#

SHELL := /usr/bin/env bash

__default__: help

help:
	@echo done $@

GYP ?= gyp
gyp:
	$(GYP) --depth=. -f xcode -DOS=ios --generator-output=./node-gles2-ios node-gles2.gyp
	$(GYP) --depth=. -f xcode -DOS=osx --generator-output=./node-gles2-osx node-gles2.gyp

