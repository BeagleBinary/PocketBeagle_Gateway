/******************************************************************************
 @file mt_msg_ini.c

 @brief TIMAC 2.0 API parse interface config from ini file

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/******************************************************************************
 Includes
*****************************************************************************/

#include "compiler.h"
#include "mt_msg.h"
#include "ini_file.h"

#include "log.h"

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/******************************************************************************
 Functions
*****************************************************************************/

/*
  Parse elements from an INI file for a message interface.

  Public function defined in mt_msg.h
*/
int MT_MSG_INI_settings(struct ini_parser *pINI,
                         bool *handled,
                         struct mt_msg_interface *pMI)
{
    int *iptr;
    bool *bptr;

    iptr = NULL;
    bptr = NULL;

    if(pINI->item_name == NULL)
    {
        return (0);
    }

    if(INI_itemMatches(pINI, NULL, "include-chksum"))
    {
        bptr = &(pMI->include_chksum);
    bgood:
        *bptr = INI_valueAsBool(pINI);
        *handled = true;
        return (0);
    }

    if( INI_itemMatches(pINI, NULL, "startup-flush") ){
        bptr = &(pMI->startup_flush);
        goto bgood;
    }

    if(INI_itemMatches(pINI, NULL, "frame-sync"))
    {
        bptr = &(pMI->frame_sync);
        goto bgood;
    }

    if(INI_itemMatches(pINI, NULL, "fragmentation-size"))
    {
        iptr = &(pMI->tx_frag_size);
    igood:
        *iptr = INI_valueAsInt(pINI);
        *handled = true;
        return (0);
    }

    if(INI_itemMatches(pINI, NULL, "retry-max"))
    {
        iptr = &(pMI->retry_max);
        goto igood;
    }

    if(INI_itemMatches(pINI, NULL, "fragmentation-timeout-msecs"))
    {
        iptr = &(pMI->frag_timeout_mSecs);
        goto igood;
    }

    if(INI_itemMatches(pINI, NULL, "intersymbol-timeout-msecs"))
    {
        iptr = &(pMI->intersymbol_timeout_mSecs);
        goto igood;
    }

    if(INI_itemMatches(pINI, NULL, "srsp-timeout-msecs"))
    {
        iptr = &(pMI->srsp_timeout_mSecs);
        goto igood;
    }

    if(INI_itemMatches(pINI, NULL, "intermsg-timeout-msecs"))
    {
        iptr = &(pMI->intermsg_timeout_mSecs);
        goto igood;
    }

    if(INI_itemMatches(pINI, NULL, "flush-timeout-msecs"))
    {
        iptr = &(pMI->flush_timeout_mSecs);
        goto igood;
    }

    if(INI_itemMatches(pINI, NULL, "len-2bytes"))
    {
        bptr = &pMI->len_2bytes;
        goto bgood;
    }

    if(INI_itemMatches(pINI, NULL, "tx-lock-timeout"))
    {
        iptr = &(pMI->tx_lock_timeout);
        goto igood;
    }
    return (0);
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
