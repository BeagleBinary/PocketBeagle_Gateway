/******************************************************************************
 @file nv_linux.h

 @brief TIMAC 2.0 API nv_linux.h NV implimentation for linux.

 Group: CMCU LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2019 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef NV_LINUX_H
#define NV_LINUX_H

/******************************************************************************
 Includes
 *****************************************************************************/

#include <nvocmp.h>
#include <stdint.h>
#include <stdbool.h>
#include "log.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

// Defines used by the nvocmp source files
#define FALSE false
#define TRUE true

/* forward declaration so that this file does not depend upon 'ini_file.h' */
struct ini_parser;

/* This is a linix specific CONFIG.H, in the embedded world this feature
 * is hard coded at compile time, in the Linux implimentation this feature
 * can be set via the configuration file. hence, the variable
 * linux_CONFIG_NV_RESTORE masquerades as a the compile time constant.
 */
extern bool linux_CONFIG_NV_RESTORE;
/*! Should the application restore state from the NV simulation file? */
#define CONFIG_NV_RESTORE linux_CONFIG_NV_RESTORE

/*! Debug log flag for basic NV activity */
#define LOG_DBG_NV_dbg  _bitN(LOG_DBG_NV_bitnum_first+0)
//#define LOG_DBG_NV_dbg  LOG_ALWAYS

/*! Debug log flag for super verbose NV activity */
#define LOG_DBG_NV_rdwr _bitN(LOG_DBG_NV_bitnum_first+1)
//#define LOG_DBG_NV_rdwr LOG_ALWAYS

// Contents of an erased Flash memory locations
#define NVOCMP_ERASEDBYTE   0xFF

// Linux simulation handle for the NVS driver
#define NVS_HANDLE (uint32_t*)1

/*!
 *  @brief forward declaration to remove dependency on NVS.h (simulation only)
 */
typedef struct
{
    void   *regionBase;   /*!< Base address of the NVS region. If the NVS
                               region is not directly accessible by the MCU
                               (such as SPI flash), this field will be set to
                               #NVS_REGION_NOT_ADDRESSABLE. */
    size_t  regionSize;   /*!< NVS region size in bytes. */
    size_t  sectorSize;   /*!< Erase sector size in bytes. This attribute is
                               device specific. */
} NVS_Attrs;

// Simulated handle for the NVS driver
typedef uint32_t* NVS_Handle;

/******************************************************************************
 Macros
******************************************************************************/

#define NVOCMP_ALERT(cond, message) { \
    if(!(cond)) LOG_printf(LOG_DBG_NV_rdwr, \
                           "NVOCMP_ALERT: message=%s\n", (message)); }
#define NVOCMP_ASSERT(cond, message) { \
    if(!(cond)) LOG_printf(LOG_DBG_NV_rdwr, \
                           "NVOCMP_ASSERT: message=%s\n", (message)); }
#define NVOCMP_FLASHACCESS(err) { (err) = 0; }

/******************************************************************************
 Functions
******************************************************************************/

/*
 * @brief Initialize the NV module as a whole.
 */
void NV_LINUX_init(void);

/*
 * @brief Load the NV simulation file from disk
 */
void NV_LINUX_load(void);

/*!
 * @brief Save the NV simulation file to disk
 */
void NV_LINUX_save(void);

/*!
 *@brief Simulation of embedded macro NVS_read
 */
int_fast16_t NV_LINUX_read(uint8_t pg, uint16_t off, uint8_t *pBuf,
                           uint16_t len);

/*!
 *@brief Simulation of embedded macro NVS_write
 */
int_fast16_t NV_LINUX_write(uint8_t dstPg, uint16_t off, uint8_t *pBuf,
                            uint16_t len);

/*!
 *@brief Simulation of embedded macro NVS_erase
 */
int_fast16_t NV_LINUX_erase(uint8_t dstPg);

/*
 * @brief Process NV items from the configuration file
 * @returns 0 success
 *
 * this is used as one of the callbacks for the INI_read() function.
 */
extern int NV_LINUX_INI_settings(struct ini_parser *pINI, bool *handled);

#ifdef __cplusplus
}
#endif

#endif /* NV_LINUX_H */

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

