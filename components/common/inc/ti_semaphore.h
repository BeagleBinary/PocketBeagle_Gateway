/******************************************************************************
 @file ti_semaphore.h

 @brief TIMAC 2.0 API Semaphore abstraction

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/*
 * Overview
 * ========
 *
 * QUESTION: Why is this file called: "ti_semaphore.h" instead of "semaphore.h"
 *
 * ANSWER: Linux defines a system header file called "semaphore.h"
 *         thus we must adapt, we cannot change Linux...
 */

#if !defined(TI_SEMAPHORE_H)
#define TI_SEMAPHORE_H

#include <stdint.h>

/*
 * @brief Create a semaphore
 * @param initial_value - initial value
 * @returns 0 on failure, non-zero semaphore handle
 */
intptr_t SEMAPHORE_create(const char *dbg_name, unsigned initial_value);

/*
 * @brief Create a semaphore
 * @param h - handle to destroy
 */
void SEMAPHORE_destroy(intptr_t h);

/*
 * @brief Add to the semaphore the value N.
 * @param h - the semaphore handle from SEMAPHORE_Init()
 * @param n - the value to add to the semaphore.
 * @returns 0 on success, negative error.
 */
int SEMAPHORE_putN(intptr_t h, unsigned n);

/*
 * @def SEMAPHORE_put
 * @brief This performs a put of 1 to the semaphore
 * @param H - the semaphore handle
 * @returns 0 on success, negative error.
 */
#define SEMAPHORE_put(H)  SEMAPHORE_putN((H), 1)

/*
 * @brief Wait for (N) from a semaphore with a timeout.
 * @param h - the semaphore handle
 * @param n - the number to geto from the semaphore
 * @param timeout_mSecs - 0=non-blocking, -1=forever, >0 blocking
 * @returns 1 upon success, 0 not aquired(ie:timeout), -1  other error.
 *
 * Note: If (timeout_mSec == 0) then, only (n=1) is a valid way to test
 */
int SEMAPHORE_waitNWithTimeout(intptr_t h, unsigned n, int timeout_mSecs);

/*
 * @brief Get 1 from a semaphore with a timeout.
 * @param h - the semaphore handle
 * @param timeout_mSecs - 0=non-blocking, -1=forever, >0 blocking
 * @returns 1 upon success, 0 if not successful and timeout was zero,
 *   negative on error or timeout
 */
#define SEMAPHORE_waitWithTimeout(h, timeout_mSecs) \
    SEMAPHORE_waitNWithTimeout((h), 1, (timeout_mSecs))

/*
 * @brief get the current value of the semaphore
 * @param h - the semaphore handle
 * @returns negative if h is invalid, otherwise 0..N current semaphore value
 */
int SEMAPHORE_inspect(intptr_t h);

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
