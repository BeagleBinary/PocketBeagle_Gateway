#############################################################
# @file Makefile
#
# @brief TIMAC 2.0 Common Library Makefile
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

# basic boiler plate makefile stuff
include ../../scripts/front_matter.mak

# What is the name of our library?
LIB_NAME=common


C_SOURCES_linux   += linux/linux_specific.c
C_SOURCES_linux   += linux/linux_uart.c

C_SOURCES_generic += src/debug_helpers.c
C_SOURCES_generic += src/fatal.c
C_SOURCES_generic += src/fifo.c
C_SOURCES_generic += src/hexline.c
C_SOURCES_generic += src/ini_file.c
C_SOURCES_generic += src/log.c
C_SOURCES_generic += src/log_ini.c
C_SOURCES_generic += src/mutex.c
C_SOURCES_generic += src/rand_data.c
C_SOURCES_generic += src/stream_common.c
C_SOURCES_generic += src/stream_file.c
C_SOURCES_generic += src/stream_mem.c
C_SOURCES_generic += src/stream_socket_client.c
C_SOURCES_generic += src/stream_socket_ini.c
C_SOURCES_generic += src/stream_socket_private.c
C_SOURCES_generic += src/stream_socket_server.c
C_SOURCES_generic += src/stream_uart_ini.c
C_SOURCES_generic += src/threads.c
C_SOURCES_generic += src/timer.c
C_SOURCES_generic += src/timer_cb.c
C_SOURCES_generic += src/ti_semaphore.c
C_SOURCES_generic += src/unix_fdrw.c

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

