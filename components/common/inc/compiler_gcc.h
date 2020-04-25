/******************************************************************************
 @file compiler_gcc.h

 @brief TIMAC 2.0 API gcc compiler specific items.

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/*
 * @def _Printf_format_string_
 * See compiler_microsoft.h
 *
 * This macro is used to identify a printf()
 * format string, when compiling for GCC
 * this macro becomes nothing and we use
 * the __attribute__ instead.
 *
 * Syntatically Microsoft tools want the 'decoration'
 * in a different place then GCC so there
 * is no means to have a single macro
 */
#define _Printf_format_string_

/*
 * @def MSVC_NO_RETURN
 * see compiler_microsoft.h
 *
 * This macro when compiling for GCC
 * becomes nothing, and we use the other
 * macro GCC_NO_RETURN
 *
 * Syntatically Microsoft tools want the 'decoration'
 * in a different place then GCC so there
 * is no means to have a single macro
 */
#define MSVC_NO_RETURN

/*
 * @def GCC_NO_RETURN
 *
 * Mark the function as no return for GCC
 * This is GCC's way of doing this
 * Microsoft has another way...
 */
#define GCC_NO_RETURN __attribute((noreturn))

/*
 * @def COMPILER_isBigEndian
 * @brief Evaluates to true (non-zero) if the compiler target is big-endian.
 */
#define COMPILER_isBigEndian   (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define COMPILER_isLittleEndian (!COMPILER_isBigEndian)

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
