/******************************************************************************
 @file stream_socket_ini.c

 @brief TIMAC 2.0 API Parse INI files to socket interfaces

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#include "compiler.h"

#include "stream.h"
#include "ini_file.h"
#include "stream_socket.h"

#include <string.h>

struct socket_cfg ALL_INI_SOCKETS[ INI_MAX_SOCKETS ];

static const struct ini_flag_name socket_types[] = {
    { .name = "server", .value = 's' },
    { .name = "client", .value = 'c' },
    { .name = NULL }
};

/*
 * Parse socket configuration information from an ini file
 * Handles both server and client sockets
 *
 * Public function defined in stream_socket.h
 */

int SOCKET_INI_settingsNth(struct ini_parser *pINI, bool *handled)
{
    unsigned nth;

    if(pINI->item_name == NULL)
    {
        /* this is the section name, we don't care about it. */
        return (0);
    }

    /* is this a SOCKET section? */
    if(!INI_isNth(pINI, "socket-", &nth))
    {
        /* Nope, then leave */
        return (0);
    }

    /* Is the number value? */
    if(nth>= INI_MAX_SOCKETS)
    {
        INI_syntaxError(pINI, "invalid-socket-index\n");
        return (-1);
    }

    return (SOCKET_INI_settingsOne(pINI, handled, &(ALL_INI_SOCKETS[nth])));
}

/*
  handle INI file callbacks for a socket interface.

  Public function in stream_socket.h
 */
int SOCKET_INI_settingsOne(struct ini_parser *pINI,
                            bool *handled,
                            struct socket_cfg *pCFG)
{
    bool is_not;
    const struct ini_flag_name *pF;
    int r;

    if(pINI->item_name == NULL)
    {
        return (0);
    }

    r = -1;
    is_not = false;

    /* match name? */
    if(INI_itemMatches(pINI, NULL, "type"))
    {
        r = 0;
        INI_dequote(pINI);
        pF = INI_flagLookup(socket_types, pINI->item_value, &is_not);
        if(pF == NULL)
        {
            INI_syntaxError(pINI, "unknown flag: %s\n", pINI->item_value);
        }
        else
        {
            pCFG->ascp = (int)(pF->value);
        }
    }

    if(INI_itemMatches(pINI, NULL, "host"))
    {
        pCFG->host = INI_itemValue_strdup(pINI);
        r = 0;
    }

    if(INI_itemMatches(pINI, NULL, "service"))
    {
        pCFG->service = INI_itemValue_strdup(pINI);
        r = 0;
    }

    if(INI_itemMatches(pINI, NULL, "devicename"))
    {
        pCFG->device_binding = INI_itemValue_strdup(pINI);
        r = 0;
    }

    if(INI_itemMatches(pINI, NULL, "server_backlog"))
    {
        pCFG->server_backlog = (unsigned)INI_valueAsU64(pINI);
        r = 0;
    }

    if(INI_itemMatches(pINI, NULL, "inet"))
    {
        r = INI_isValueInt(pINI, &(pCFG->inet_4or6));
        if(r)
        {
            if((pCFG->inet_4or6 == 4) || (pCFG->inet_4or6 == 6))
            {
                r = 0;
                goto done;
            }
            else
            {
                goto bad_inet46;
            }
        }
        else
        {
            if(0 == strcmp(pINI->item_value, "any"))
            {
                r = 0;
                pCFG->inet_4or6 = 0;
                goto done;
            }
            else
            {
            bad_inet46:
                INI_syntaxError(pINI,
                                "inet must be 4, 6 or any, not: %s\n",
                                pINI->item_value);
                return (-1);
            }
        }
    }
done:
    if(r == 0)
    {
        /* We handle it */
        *handled = true;
    }
    else
    {
        INI_syntaxError(pINI, "unknown socket item\n");
    }
    return (r);
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
