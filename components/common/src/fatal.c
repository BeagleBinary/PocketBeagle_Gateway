/******************************************************************************
 @file fatal.c

 @brief TIMAC 2.0 API Handle fatal exit conditions

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#include <compiler.h>

#include "fatal.h"
#include "log.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include "hlos_specific.h"

/*
 * printf() a fatal message and exit.
 *
 * Public function defined in fatal.h
 */
void FATAL_printf(_Printf_format_string_ const char *fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    FATAL_vprintf(fmt, ap);
    /* UNREACHABLE: va_end(ap); */
}

/*
 * building block for FATAL_printf()
 *
 * Public function defined in fatal.h
 */
void FATAL_vprintf(const char *fmt, va_list ap)
{
    va_list ap_copy;
    va_copy(ap_copy, ap);

    LOG_vprintf(LOG_FATAL, fmt, ap);
    vfprintf(stderr, fmt, ap_copy);
    _FATAL_exit(1);
}

/*
 * print a message followed by a unix error number and message
 *
 * Public function defined in fatal.h
 * see fatal.h
 */
void FATAL_perror(const char *msg)
{
    const char *cp;
    cp = strerror(errno);
    if(cp == NULL)
    {
        cp = "";
    }
    FATAL_printf("%s: (%d) %s\n", msg, errno, cp);
}

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

