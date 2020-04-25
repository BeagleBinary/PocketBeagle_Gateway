#############################################################
# @file library.mak
#
# @brief TIMAC 2.0 Library Makefile fragment (Front portion)
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

#========================================
#
# ===================
# Theory of Operation
# ===================
#
# A detailed description is in "front_matter.mak"
#


#========================================
# Error checking
#----------------------------------------
# Make sure the libname was specified
ifeq (x${LIB_NAME}x,xx)
$(error LIB_NAME is not specified)
endif


# convert *.c in varous forms into objs/host/*.o
#             or objs/bbb/*.o
# 
# we also need to change "linux/*.c" to "*.c"
# and "src/*.c" to just "*.c"
# We do this in 2 steps
_1_C_SOURCES=${C_SOURCES:linux/%.c=%.c}
_2_C_SOURCES=${_1_C_SOURCES:src/%.c=%.c}
# Now convert
OBJFILES=${_2_C_SOURCES:%.c=${OBJDIR}/%.o}

#========================================
# What is the name of our lib file we are creating
# ie:  foo ->  objs/host/libfoo.a
# or           objs/bbb/libfoo.a
LIBFILE=${OBJDIR}/lib${LIB_NAME}.a

ifeq (${ARCH},all_arches)
# Are we building all
_libfile.all: ${ALL_ARCHES:%=_libfile.%}

# do this recursivly
_libfile.%:
	${MAKE} ARCH=$* _libfile
else
# the lib file generically
_libfile: ${LIBFILE}

# the actual file
${LIBFILE}: bbb_compiler_check generated_files ${OBJFILES} force 
	${HIDE}rm -f $@
	${HIDE}echo "Creating Lib: ${LIBFILE}"
	${HIDE}${AR_EXE} qc $@ ${OBJFILES}

clean::
	${HIDE}rm -f ${LIBFILE}

endif

#========================================
# We might have some uint test app
# build test apps, generically
_TEST_MAKEFILES=${wildcard *_test.mak}

testapps: ${_TEST_MAKEFILES:%=testapp_%}

testapp_%:
	${MAKE} -f $* ${MAKEFLAGS}



#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: makefile-gmake
#  End:
#  vim:set  filetype=make

