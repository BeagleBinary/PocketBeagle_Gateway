#!/bin/bash
#############################################################
# @file run_gateway.sh
#
# @brief TIMAC 2.0 run_gateway.sh, used by run_demo.sh to launch gateway
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

nodejs=`which nodejs`
arch=`uname -m`

if [ x"$nodejs"x != "xx" ]
then
    # great, we have this
    :
else
    if [ x"${arch}"x == x"armv7l"x ]
    then
	# This name should be changed...
	# however it has not been done yet
	# SEE:
	#  https://lists.debian.org/debian-devel-announce/2012/07/msg00002.html
	nodejs=`which node`
    fi
fi

if [ x"$nodejs"x == xx ]
then
	echo "Cannot find node-js appplication, is it installed?"
	exit 1
fi

if [ ! -f $nodejs ]
then
	echo "Cannot find node-js application, is it installed?"
	exit 1
fi

if [ ! -x $nodejs ]
then
    echo "Cannot find node-js application, is it installed?"
    exit 1
fi

PID=`pidof $nodejs`

if [ "x${PID}x" != "xx" ]
then
    kill -9 ${PID}
fi


$nodejs ./gateway.js &
PID=$!
# Wait a couple seconds for it to get started
# or ... for it to exit
sleep 2

if ps -p $PID > /dev/null
then
    echo "Gateway is running as Process id: ${PID}"
    exit 0
else
    echo "Cannot start gateway application"
    exit 1
fi
