/******************************************************************************
 @file mutex.h

 @brief TIMAC 2.0 API generic abstract mutex implimentation

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#if !defined(MUTEX_H)
#define MUTEX_H

#include <stdbool.h>
#include <stdint.h>

/*!
 * @brief Create a mutex
 * @param name - for debug purposes
 * @returns mutex handle for later use
 *
 * Implimentation note: this is not a named Linux semaphore
 */
intptr_t MUTEX_create(const char *name);
/*!
 * @brief release resources associated with a mutex
 * @param h - mutex handle from MUTEX_Create()
 */
void MUTEX_destroy(intptr_t h);

/*!
 * @brief Lock a mutex with timeout
 * @param h - mutex handle from MUTEX_create()
 * @param mSecTimeout - negative=forever, 0=non-blocking >0 timeout value
 *
 * @returns If timeout=0, then 1=success, 0=fail, -1=other error
 * @returns If timeout > 0, then 1 success, other failure
 */
int MUTEX_lock(intptr_t h, int mSecTimeout);

/*!
 * @brief Unlock a mutex
 * @param h - mutex handle from MUTEX_create()
 * @returns 0 success, negative if not a valid mutex handle
 */
int MUTEX_unLock(intptr_t h);

/*!
 * @brief test, determine if a mutex is locked or not
 *
 * @param h - mutex handle from MUTEX_create()
 * @return boolean, true if locked
 */

bool MUTEX_isLocked(intptr_t h);

/*!
 * @brief returns the name of the current locker
 *
 * Or if not locked, the string "none"
 */
const char *MUTEX_lockerName(intptr_t h);

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
