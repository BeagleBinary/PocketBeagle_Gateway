#############################################################
# @file Makefile
#
# @brief TIMAC 2.0 "google protobuf" makefile
#
# This makefile can download, build and install the google
# protobuf compiler and the "protobuf-c" extension
#
# Group: WCS LPC
# $Target Device: DEVICES $
#
#############################################################
# $License: BSD3 2016 $
#############################################################
# $Release Name: PACKAGE NAME $
# $Release Date: PACKAGE RELEASE DATE $
#############################################################

#----------------------------------------
# NOTE:
#  This was tested/documented Thu Mar 31 16:15:04 PDT 2016
#
#  The current GIT commits where:
#     protobuf-c: 006d69bd8481738323c58af5cd4c8477d09eb9f5
#     protobuf:   5274d6ee314a237f6626b5b4633c9f87574ad46d
#
# Our (TI) expectation is, this will continue to work with
# the current (head) version of the protobuf tools.
# 
# However, if something goes wrong - you can always revert to
# this 'known-good-state' by using the above date/time and/or
# by using last known GIT commit IDs.
#
#----------------------------------------

default:
	@echo "Please use 'make all' to build everything"
	@echo "Or make <something>.protobuf"
	@echo "Or make <something>.proto-c"
	@echo ""
	@echo "Where <something> is one of: checkout autogen config build install"
	@echo ""
	@exit 1

HERE := $(shell pwd)

PREFIX=${HERE}/install

checkout.protobuf:
	git clone https://github.com/google/protobuf.git
	cd ./protobuf && git checkout 5274d6ee314a237f6626b5b4633c9f87574ad46d
	cd ..
autogen.protobuf:
	cd ./protobuf && sh ./autogen.sh

config.protobuf:
	cd ./protobuf && ./configure --prefix=$(PREFIX)

build.protobuf:
	cd ./protobuf && make

install.protobuf:
	cd ./protobuf && make install

all.protobuf: \
	checkout.protobuf autogen.protobuf config.protobuf build.protobuf install.protobuf


checkout.proto-c:
	git clone https://github.com/protobuf-c/protobuf-c.git
	cd protobuf-c && git checkout 006d69bd8481738323c58af5cd4c8477d09eb9f5
	cd ..
autogen.proto-c:
	cd protobuf-c && ./autogen.sh

configure.proto-c:
	cd protobuf-c && PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig ./configure --prefix=${PREFIX}

build.proto-c:
	cd protobuf-c && make

install.proto-c:
	cd protobuf-c && make install


all.proto-c: \
	checkout.proto-c autogen.proto-c configure.proto-c build.proto-c install.proto-c

all: all.protobuf all.proto-c

only.clone:  checkout.protobuf  checkout.proto-c

dist-clean:
	rm -rf install
	rm -rf protobuf
	rm -rf protobuf-c

#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: makefile-gmake
#  End:
#  vim:set  filetype=make

