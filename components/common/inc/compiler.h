/******************************************************************************
 @file compiler.h

 @brief TIMAC 2.0 API Handle different compiler types macros, etc.

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#if defined(__linux__)
#include "compiler_gcc.h"
#endif

#if defined(_MSC_VER)
#include "compiler_microsoft.h"
#endif

/*
 * standard "free()" function inside a wrapper to
 * elminate the '-Wcast-qual' warning for gcc.
 *
 * And other warnings ... from other compilers.
 */
void free_const(const void *vp);

/*
 *  ========================================
 *  Texas Instruments Micro Controller Style
 *  ========================================
 *  Local Variables:
 *  mode: c
 *  c-file-style: "bsd"
 *  tab-width: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 *  End:
 *  vim:set  filetype=c tabstop=4 shiftwidth=4 expandtab=true
 */
