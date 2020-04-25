#############################################################
# @file app.mak
#
# @brief TIMAC 2.0 Aplication Makefile fragment
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
#
#========================================
# Error check
#----------------------------------------
# Ensure that a NAME is specified..
ifeq (x${APP_NAME}x,xx)
$(error APP_NAME is not specified)
endif


#========================================
# What is the name of the 'appfile' we are creating
# Example, if the APP_NAME=foo, the result could be
#    host_foo
# or  bbb_foo
#
APPFILE=${ARCH}_${APP_NAME}

#========================================
# convert *.c in varous forms into objs/host/*.o
#             or objs/bbb/*.o
# 
# we also need to change "linux/*.c" to "*.c"
# and "src/*.c" to just "*.c"
# We do this in 2 steps
_1_C_SOURCES=${C_SOURCES:linux/%.c=%.c}
_2_C_SOURCES=${_1_C_SOURCES:src/%.c=%.c}
# Now convert to ".o" files
OBJFILES=${_2_C_SOURCES:%.c=${OBJDIR}/%.o}

#========================================
# if ARCH is specified and is all arches..

ifeq (${ARCH},all_arches)

# Then build all of the arches
_app.all: ${ALL_ARCHES:%=_app.%}

# Recursively build each ARCH target.
_app.%:
	${recursive_MAKE} ARCH=$* _app

else

# specific arch is specified, so just build the app
_app: ${APPFILE}

#========================================
# Do we have any pre/post link commands?
# Supply reasonable defaults ... if not specified.
PRE_LINK_CMD  ?= /bin/true
POST_LINK_CMD ?= /bin/true

# Perform the prelink step
_prelink:
	${HIDE}${PRE_LINK_CMD}

# Do we need any extra libs?
# Yes, the host apps are pthread based..
EXTRA_APP_LIBS += -lpthread

# this builds the "host_foo" or "bbb_foo" app
# STEP 1: the OBJECT directory
# STEP 2: any generated files
# STEP 3: The object files
# STEP 4: Any pre-link step
# STEP 5: We purposely force the link to occur
# STEP 6: We do the POST LINK as part of the rule.
# 
${APPFILE}: bbb_compiler_check ${OBJDIR} generated_files ${OBJFILES}   _prelink force 
	${HIDE}rm -f $@
	${HIDE}echo "Creating App: (${ARCH}) ${APPFILE}"
	${HIDE}${GCC_EXE} -g ${ARCH_LDFLAGS} -o $@ ${OBJFILES} ${APP_LIBDIRS:%=-L%} ${APP_LIBS:lib%.a=-l%} ${EXTRA_APP_LIBS}
	${HIDE}echo "Done."
	${HIDE}${POST_LINK_CMD}

# Cleanup...
clean::
	${HIDE}rm -f ${APPFILE}


#========================================
# Create a symbol table.
_app.sym: ${APPFILE}.sym
#
${APPFILE}.sym: ${APPFILE} force
	${HIDE}rm -f $@
	${NM_EXE} -demangle -o $@ $<

# cleanup
clean::
	${HIDE}rm -f ${APPFILE}.sym

#========================================
# Create a disassembly listing
_app.dis: ${APPFILE}.dis 
#
${APPFILE}.dis: ${APPFILE} force
	${HIDE}rm -f $@
	${OBJDUMP_EXE} -d -demangle -o $@ $<

# cleanup
clean::
	${HIDE}rm -f ${APPFILE}.dis

endif




#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: makefile-gmake
#  End:
#  vim:set  filetype=make

