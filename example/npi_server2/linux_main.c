/******************************************************************************
 @file linux_main.c

 @brief TIMAC 2.0 API Linux "main" for the NPI server application

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#include "compiler.h"
#include "npi_server2.h"

#include "ini_file.h"       /* this reads our ini file */
#include "log.h"            /* our logging scheme */
#include "mt_msg.h"
#include "mt_msg_dbg.h"
#include "timer.h"
#include "fatal.h"
#include "stream_socket.h"  /* we use a socket in our app */
#include "stream_uart.h"    /* and a uart. */

#include <stdio.h>
#include <stdlib.h>

const struct ini_flag_name * const log_flag_names[] = {
    log_builtin_flag_names,
    mt_msg_log_flags,
    /* terminate */
    NULL
};

/*!
 * @brief Handle any config file settings for the uart.
 *
 * @param pINI - ini file parse info
 * @param handled - set to true if the item was handled
 */
static int my_UART_INI_settings(struct ini_parser *pINI, bool *handled)
{
    int r;

    r = 0;
    if(INI_itemMatches(pINI, "uart-cfg", NULL))
    {
        r = UART_INI_settingsOne(pINI, handled, &my_uart_cfg);
    }
    return r;
}

static int my_SOCKET_INI_settings(struct ini_parser *pINI, bool *handled)
{
    int r;

    r = 0;
    if(INI_itemMatches(pINI, "socket-cfg", NULL))
    {
        r = SOCKET_INI_settingsOne(pINI, handled, &my_socket_cfg);
    }
    return r;
}

static int my_MT_MSG_INI_settings(struct ini_parser *pINI, bool *handled)
{
    int r;

    r = 0;
    if(INI_itemMatches(pINI, "socket-interface", NULL))
    {
        r = MT_MSG_INI_settings(pINI, handled, &socket_interface_template);
    }

    if(INI_itemMatches(pINI, "uart-interface", NULL))
    {
        r = MT_MSG_INI_settings(pINI, handled, &common_uart_interface);
    }
    return r;
}

static int my_APP_settings(struct ini_parser *pINI, bool *handled)
{
    if(!INI_itemMatches(pINI, "application", NULL))
    {
        return 0;
    }

    if(INI_itemMatches(pINI,NULL,"msg-dbg-data"))
    {
        struct mt_msg_dbg **ppDbg;

        /* append at end of list */
        ppDbg = &(ALL_MT_MSG_DBG);
        while( *ppDbg ){
            ppDbg = &((*ppDbg)->m_pNext);
        }
        INI_dequote(pINI);
        *ppDbg = MT_MSG_dbg_load(pINI->item_value);
        *handled = true;
        return 0;
    }
    return 0;
}

static ini_rd_callback * const ini_cb_table[] = {
    LOG_INI_settings,
    my_UART_INI_settings,
    my_SOCKET_INI_settings,
    my_MT_MSG_INI_settings,
    my_APP_settings,
    /* Terminate list */
    NULL
};

static int cfg_callback(struct ini_parser *pINI, bool *handled)
{
    int x;
    int r;

    for(x = 0 ; ini_cb_table[x] ; x++)
    {
        r = (*(ini_cb_table[x]))(pINI, handled);
        if(*handled)
        {
            return r;
        }
    }
    /* let the system handle it */
    return 0;
}

int main(int argc, char **argv)
{
    int r;
    const char *cfg_filename;

    cfg_filename = "npi_server2.cfg";

    switch(argc)
    {
    default:
        fprintf(stderr, "Usage: %s [CONFIGFILE]\n", argv[0]);
        fprintf(stderr, "\n");
        fprintf(stderr, "Default CONFIGFILE = %s\n", cfg_filename);
        exit(1);
    case 1:
        /* use default */
        break;
    case 2:
        cfg_filename = argv[1];
        break;
    }

    /* Basic initialization */
    SOCKET_init();
    STREAM_init();
    TIMER_init();
    LOG_init("/dev/stderr");
    /* we want these logs to begin with */
    log_cfg.log_flags = LOG_FATAL | LOG_WARN | LOG_ERROR;

    APP_defaults();

    /* Rad our configuration file */
    r = INI_read(cfg_filename, cfg_callback, 0);
    if(r != 0)
    {
        FATAL_printf("Failed to read cfg file\n");
    }

    APP_main();

    exit(0);
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
