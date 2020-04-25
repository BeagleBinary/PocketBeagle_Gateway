#############################################################
# @file Makefile
#
# @brief TIMAC 2.0 Multi-page NV (nonvolatile) Library Makefile
#
# Group: CMCU LPC
# $Target Device: DEVICES $
#
#############################################################
# $License: BSD3 2019 $
#############################################################
# $Release Name: PACKAGE NAME $
# $Release Date: PACKAGE RELEASE DATE $
#############################################################

# By default, we make the library file
_default: _libfile

# basic boiler plate makefile stuff
include ../../scripts/front_matter.mak

#include files in common
CFLAGS += -I../common/inc
CFLAGS += -DNV_LINUX
CFLAGS += -DNVOCMP_POSIX_MUTEX
CFLAGS += -DNVOCMP_NVPAGES=4

# What is the name of our library?
LIB_NAME=nv

# What source files go into the library?
C_SOURCES += crc.c
C_SOURCES += nvocmp.c
C_SOURCES += nv_linux.c


# And include the final library portion
include ../../scripts/library.mak

#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: makefile-gmake
#  End:
#  vim:set  filetype=make

