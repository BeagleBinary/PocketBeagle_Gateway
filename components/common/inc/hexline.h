/******************************************************************************
 @file hexline.h

 @brief TIMAC 2.0 API Debug hex dump helper functions

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#if !defined(HEXLINE_H)
#define HEXLINE_H

#include <stddef.h>
#include <stdint.h>

/*!
 * @brief HEXLINE_working variables
 */
struct hexline {
    /*! Address to be printed */
    uint64_t    addr;

    /*! Pointer to the data to be printed */
    const void *pBytes;

    /*! How many bytes to print */
    size_t      nbytes;

    /*! How many are complete */
    size_t      ndone;

    /* ! Working buffer (your text will be put here) */
    char buf[90];
};

/*!
 * @brief Initialize a HEXLINE Working buffer
 *
 * @param pH - pointer to the hexline structure
 * @param addr - address field initializer
 * @param pBytes - pointer to the data bytes to dump
 * @param nbytes - count of bytes to dump
 *
 */
void HEXLINE_init(struct hexline *pH,
                   uint64_t addr,
                   const void *pBytes,
                   size_t nbytes);

/*!
 * @brief Format one line (up to 16 bytes) of hex dump data.
 *
 * @param pHexLine - structure initalized by HEXLINE_Init()
 *
 * Example
 * \code
 *
 *    HEXLINE_Init(&h, 0x1234000, buffer, 1000);
 *
 *    while(h.ndone < h.nbyes){
 *        HEXLINE_format(&h);
 *        printf("%s\n", h.buf);
 *   }
 * \endcode
 */
void HEXLINE_format(struct hexline *pHexLine);

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

