/******************************************************************************
 @file api_mac_linux.h

 @brief TIMAC 2.0 API Linux implimentation header for api_mac

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#if !defined(API_MAC_LINUX_H)
#define API_MAC_LINUX_H

/******************************************************************************
 Constants and Definitions
 *****************************************************************************/

/*! Debug Log flag specific to api-mac */
#define LOG_DBG_API_MAC_wait      _bitN(LOG_DBG_API_MAC_bitnum_first+0)
#define LOG_DBG_API_MAC_datastats _bitN(LOG_DBG_API_MAC_bitnum_first+1)


/* TX Options (from the embedded) */
/*! Acknowledged transmission.  The MAC will attempt to retransmit
  the frame until it is acknowledged */
#define MAC_TXOPTION_ACK            0x0001
/*! GTS transmission (unused) */
#define MAC_TXOPTION_GTS            0x0002
/*! Indirect transmission.  The MAC will queue the data and wait
  for the destination device to poll for it.  This can only be used
  by a coordinator device */
#define MAC_TXOPTION_INDIRECT       0x0004
/*! This proprietary option forces the pending bit set for direct
  transmission */
#define MAC_TXOPTION_PEND_BIT       0x0008
/*! This proprietary option prevents the frame from being retransmitted */
#define MAC_TXOPTION_NO_RETRANS     0x0010
/*! This proprietary option prevents a MAC_MCPS_DATA_CNF
  event from being sent for this frame */
#define MAC_TXOPTION_NO_CNF         0x0020
/*! Use PIB value MAC_ALT_BE for the minimum backoff exponent */
#define MAC_TXOPTION_ALT_BE         0x0040
/*! Use the power and channel values in macDataReq_t
  instead of the PIB values */
#define MAC_TXOPTION_PWR_CHAN       0x0080
/*! Special transmit for Green Power for CC2530 only. Must be direct */
#define MAC_TXOPTION_GREEN_PWR      0x0100

/******************************************************************************
 Variables
 *****************************************************************************/

/*!
  @brief the application points this to the message interface structure.
 */
extern struct mt_msg_interface *API_MAC_msg_interface;

/*! How long should the API-MAC wait when there are no messsages to process. */
extern int ApiMacLinux_areq_timeout_mSecs;
/*! Default value if not overridden via configuration file */
#ifdef IS_HEADLESS
#define DEFAULT_ApiMacLinux_areq_timeout_mSecs  (10 * 1000)
#else
/*! timeout every 500ms to allow UI polling */
#define DEFAULT_ApiMacLinux_areq_timeout_mSecs  (300)
#endif //HEADLESS

extern struct mt_version_info MT_DEVICE_version_info;

#endif // API_MAC_LINUX_H

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

