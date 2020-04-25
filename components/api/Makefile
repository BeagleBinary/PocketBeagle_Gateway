#############################################################
# @file Makefile
#
# @brief TIMAC 2.0 makefile for the Linux API MAC and MT Layer 
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

# By default, we make the library file
_default: _libfile

CFLAGS += -I../common/inc

# basic boiler plate makefile stuff
include ../../scripts/front_matter.mak

# What is the name of our library?
LIB_NAME=apimac


C_SOURCES_linux += src/mt_msg_ini.c
C_SOURCES_linux += src/mt_msg.c
C_SOURCES_linux += src/api_mac.c
C_SOURCES_linux += src/mt_msg_dbg_core.c
C_SOURCES_linux += src/mt_msg_dbg_load.c
C_SOURCES_generic =

C_SOURCES += ${C_SOURCES_linux}
C_SOURCES += ${C_SOURCES_generic}


# And include the final library portion
include ../../scripts/library.mak

#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: makefile-gmake
#  End:
#  vim:set  filetype=make

