/******************************************************************************
 @file fatal.h

 @brief TIMAC 2.0 API handle fatal errors

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#if !defined(FATAL_H)
#define FATAL_H

#include "compiler.h"

/*
 * OVERVIEW
 * =========
 *
 * These functions print (log) a message and exit the application
 */
#include <stdarg.h>

/*!
 * @brief print a message like the unix perror() function and exit
 * @param msg - the message to print
 * @returns - this function does not return it exits the application
 *
 * See: "man 3 perror" for more details behind the name "perror"
 */
void MSVC_NO_RETURN FATAL_perror(const char *msg) GCC_NO_RETURN;

/*!
 * @brief printf() a fatal error message and exit.
 * @param fmt - the printf() format message
 * @returns - this function does not return it exits the application
 */
void MSVC_NO_RETURN FATAL_printf(_Printf_format_string_ const char *fmt, ... )
    GCC_NO_RETURN __attribute__((format (printf,1,2)));

/*!
 * @brief FATAL_printf() building block function.
 * @param fmt - the printf() format message
 * @returns - this function does not return it exits the application
 */
void MSVC_NO_RETURN FATAL_vprintf(const char *fmt, va_list ap) GCC_NO_RETURN;

#endif

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
