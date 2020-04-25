/******************************************************************************
 @file npi_server2.h

 @brief TIMAC 2.0 API npi (network processor interface) v2 primary app header.

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#include "stream.h"
#include "stream_uart.h"
#include "stream_socket.h"
#include "log.h"

#include "mt_msg.h"

extern struct uart_cfg my_uart_cfg;
extern struct socket_cfg my_socket_cfg;

extern struct mt_msg_interface common_uart_interface;
extern struct mt_msg_interface socket_interface_template;

void APP_defaults(void);

void APP_main(void);

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

