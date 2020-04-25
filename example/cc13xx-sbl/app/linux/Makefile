_default: host
#/******************************************************************************
#
# @file Makefile
#
# @brief CC13xx Bootloader Example Application
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

SBU_REV= "0.1"

INCLUDE = -I$(PROJ_DIR)../../cc13xxdnld -I$(PROJ_DIR)../../platform/linux 

klockwork:
	rm -f ./kw.output
	${MAKE} KLOCKWORK=true remake host

ifdef KLOCKWORK
HERE := $(shell pwd)
CC_HOST= kwwrap -o ${HERE}/kw.output gcc
else
CC_HOST= gcc
endif

#----------------------------------------
# Location of the BBB compiler, the Processor SDK
#----------------------------------------
BBB_SDK=${HOME}/ti-processor-sdk-linux-am335x-evm-06.01.00.08

#----------------------------------------
# Hint/Example: Make a symbolic link like this:
#      bash$ cd $HOME
#      bash$ rm -f cur_sdk
#      bash$ ln -s ti-processor-sdk-linux-am335x-evm-05.03.00.07 cur_sdk
# Thus:
#    $HOME/cur_sdk -> $HOME/ti-processor-sdk-linux-am335x-evm-05.03.00.07
#----------------------------------------
# Within the Processor SDK, we find the compiler
BBB_PREFIX=${BBB_SDK}/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/arm-linux-gnueabihf-
CC_BBB = ${BBB_PREFIX}gcc


# Sanity check, is the compiler present? Y or N?
bbb_compiler_check:
	@if [ ! -x ${CC_BBB} ] ; \
	then \
		echo "ERROR: Cannot find BBB Compiler: ${CC_BBB}" ; \
		echo "ERROR: Please update variable: BBB_PREFIX"  ; \
		echo "in the file SDK_ROOT/cc13xx-sbl/app/linux/Makefile"; \
		echo "ERROR: Cannot continue" ; \
		exit 1 ; \
	fi

.PHONY:: bbb_compiler_check 

CFLAGS= -c -Wall -g -std=gnu99
# Unconnemt to enable two wire UART interface
#CFLAGS += -DSBL_TWO_WIRE

PROJ_DIR=

OBJ_HOST=objs/host
OBJ_BBB =objs/bbb

bbb: bbb_cc13xx-sbl
host: host_cc13xx-sbl

.PHONY:: bbb host

all: bbb_cc13xx-sbl host_cc13xx-sbl

.PHONY:: all

.PHONY:: bbb_cc13xx-sbl host_cc13xx-sbl

${OBJ_HOST}:
	mkdir -p $@
${OBJ_BBB}:
	mkdir -p $@

host_cc13xx-sbl: ${OBJ_HOST}/main.o ${OBJ_HOST}/cc13xxdnld.o ${OBJ_HOST}/sblUart.o
	$(CC_HOST) -o host_cc13xx-sbl ${OBJ_HOST}/main.o ${OBJ_HOST}/cc13xxdnld.o ${OBJ_HOST}/sblUart.o

# rule for file "cc13xxdnld.o".
${OBJ_HOST}/main.o: main.c ${OBJ_HOST}
	$(CC_HOST) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)main.c -o $@

# rule for file "cc13xxdnld.o".
${OBJ_HOST}/cc13xxdnld.o: ../../cc13xxdnld/cc13xxdnld.c  ${OBJ_HOST}
	$(CC_HOST) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)../../cc13xxdnld/cc13xxdnld.c  -o $@

# rule for file "rpcTransport.o".
${OBJ_HOST}/sblUart.o: sblUart.h sblUart.c  ${OBJ_HOST}
	$(CC_HOST) $(CFLAGS) $(INCLUDE) $(DEFS) sblUart.c -o $@

bbb_cc13xx-sbl: bbb_compiler_check ${OBJ_BBB}/main.o ${OBJ_BBB}/cc13xxdnld.o ${OBJ_BBB}/sblUart.o 
	$(CC_BBB) -o bbb_cc13xx-sbl ${OBJ_BBB}/main.o ${OBJ_BBB}/cc13xxdnld.o ${OBJ_BBB}/sblUart.o

# rule for file "cc13xxdnld.o".
${OBJ_BBB}/main.o: main.c ${OBJ_BBB}
	$(CC_BBB) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)main.c  -o $@

# rule for file "cc13xxdnld.o".
${OBJ_BBB}/cc13xxdnld.o: ../../cc13xxdnld/cc13xxdnld.c ${OBJ_BBB}
	$(CC_BBB) $(CFLAGS) $(INCLUDE) $(DEFS) $(PROJ_DIR)../../cc13xxdnld/cc13xxdnld.c  -o $@

# rule for file "rpcTransport.o".
${OBJ_BBB}/sblUart.o: sblUart.h sblUart.c ${OBJ_BBB}
	$(CC_BBB) $(CFLAGS) $(INCLUDE) $(DEFS) sblUart.c  -o $@

# rule for cleaning files generated during compilations.
clean:
	/bin/rm -f host_* bbb_* 
	/bin/rm -rf objs

remake: clean _default

