/******************************************************************************
 @file stream_uart.h

 @brief TIMAC 2.0 API Header for streams that are uarts

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#if !defined(STREAM_UART_H)
#define STREAM_UART_H

#include "bitsnbits.h"

/*!
 * @struct uart_cfg
 */
struct uart_cfg {
    /*! device name, linux example: "/dev/ttyUSB0" */
    const char *devname;

    /*! Example: cfg.baudrate = 115200 */
    int baudrate;

    /*! See STREAM_UART_FLAG_xxxx below */
    int open_flags;

};

/*! Use a linux thread to read this device into a fifo */
#define STREAM_UART_FLAG_rd_thread    _bit0
/*! use hardware handshake */
#define STREAM_UART_FLAG_hw_handshake _bit1
/*! Basic IO, no thread no nothing blocking IO */
#define STREAM_UART_FLAG_default     0

/*!
 * @brief Create a stream that is UART based
 *
 * @param pCFG - the uart configuration to use.
 *
 * @returns non-zero on success
 */
intptr_t STREAM_createUart(const struct uart_cfg *pCFG);

/*!
 * @brief Test if this stream is a uart or not
 * @param h - the io stream to test
 * @returns true if io stream is a uart.
 */
bool STREAM_isUart(intptr_t h);

/* forward decloration */
struct ini_parser;

/*
 * @brief Handle INI File settings for a uart.
 * @param pINI - ini file parse information
 * @param handled - set to true if this settings was handled.
 * @returns negative on error
 *
 * This parses a UART configuration like this:
 *     [uart-N]
 *        devname /some/name
 *        baudrate VALUE
 *        hw_handshake bool
 *        flag name
 *        flag name .. [flags can be repeated]
 *
 * Where [uart-N] ranges from uart-0 to uart-MAX_UARTS
 * and will populate the array ALL_UART_CFGS[]
 */
int UART_INI_settingsNth(struct ini_parser *pINI, bool *handled);

/*
 * @brief building block for UART_INI_settingsNth()
 *
 * This function ignores the section name.
 *
 * @param pINI - ini file parse information
 * @param handled - set to true if this settings was handled.
 * @param pCfg - the configuration to structure to fill in.
 * @returns negative on error
 *
 * Example:
 * \code
 * struct stream_cfg my_cfg;
 *
 * int my_settings(struct ini_parser *pINI, bool *handled)
 * {
 *     // Handle [server] sections
 *     if(!INI_itemMatches(pINI, "server", NULL)){
 *         return 0;
 *     }
 *     return SOCKET_INI_settingsOne(pINI, handled, &my_cfg);
 * }
 * \endcode
 */
int UART_INI_settingsOne(struct ini_parser *pINI,
                          bool *handled,
                          struct uart_cfg *pCfg);

/*
 * @def INI_MAX_UARTS
 * @hideinitializer
 * @brief  number of uarts supported by the built in ini reader.
 */
#define INI_MAX_UARTS 10

/* @var ALL_INI_UARTS
 * @brief Array of all uarts supported by the INI file
 */
extern struct uart_cfg ALL_INI_UARTS[INI_MAX_UARTS];

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
