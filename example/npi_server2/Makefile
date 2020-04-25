#############################################################
# @file Makefile
#
# @brief TIMAC 2.0 Linux makefile for the NPI (network processor interface) V2 application
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

_default: _app

include ../../scripts/front_matter.mak

APP_NAME=npi_server2

COMPONENTS_HOME=../../components

CFLAGS += -I${COMPONENTS_HOME}/common/inc
CFLAGS += -I${COMPONENTS_HOME}/api/inc
#CFLAGS += -DIS_HEADLESS

C_SOURCES = 
C_SOURCES += linux_main.c
C_SOURCES += app_main.c

APP_LIBS    += libapimac.a
APP_LIBS    += libcommon.a

APP_LIBDIRS += ${COMPONENTS_HOME}/common/${OBJDIR}
APP_LIBDIRS += ${COMPONENTS_HOME}/api/${OBJDIR}


include ../../scripts/app.mak

#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: makefile-gmake
#  End:
#  vim:set  filetype=make


