#!/bin/bash
#############################################################
# @file build_all.sh
#
# @brief TIMAC 2.0 Build All script
#
# This shell script will "build the world" everything in order.
# It accepts a few parameters, examples:
#
#    bash$ bash ./build_all.sh              <- defaults to host build
#    bash$ bash ./build_all.sh host         <- host only
#    bash$ bash ./build_all.sh bbb          <- bbb target
#    bash$ bash ./build_all.sh clean        <- erases everything
#    bash$ bash ./build_all.sh remake       <- 'clean' and then 'all'
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

# Exit if there was a problem
set -e

function usage()
{
	echo "usage $0 TARGET"
	echo ""
	echo "Where TARGET is one of:  host, bbb"
	echo ""
	echo "The BBB options require appropriate tools"
	echo "be made in `pwd`/scripts/front_matter.mak"
	echo "And in `pwd`/example/cc13xx-sbl/app/linux/Makefile"
	echo ""
}

case $# in
0)
	target=host
	;;
1)
	target=$1
	;;
*)
	echo "Wrong number of parameters, $#"
	usage
	exit 1
	;;
esac

case $target in
clean)
	# ok
	;;
host)
	# ok
	;;
bbb)
	# ok
	;;
klockwork)
	# ok
	;;
remake)
	# ok
	;;
*)
	echo "Unknown target: $target"
	usage
	exit 1
esac

pushd components/common
    make $target |& tee $target.common.log
popd

pushd components/nv
    make $target |& tee $target.nv.log
popd

pushd components/api
    make $target |& tee $target.api.log
popd

pushd example/npi_server2
    make $target |& tee $target.npi_server2.log
popd

pushd example/collector
    make $target |& tee $target.collector.log
popd

pushd example/cc13xx-sbl/app/linux 
    make $target |& tee $target.bootloader.log
popd

#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: sh
#  End:
#  vim:set  filetype=sh

