/******************************************************************************
 @file threads.h

 @brief TIMAC 2.0 API generic thread abstraction header

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#if !defined(THREADS_H)
#define THREADS_H

#include "bitsnbits.h"
#include <stdint.h>
#include <stdbool.h>

/*!
 * @typedef thread_func_t - thread function
 * @param cookie - a parameter for the thread function
 * @return a thread exit value can be integer or pointer
 */
typedef intptr_t threadfunc_t(intptr_t cookie);

#define THREAD_FLAGS_DEFAULT  0 /*! default flags when creating a thread */
#define THREAD_FLAGS_JOINABLE _bit0 /*! make the thread joinable */

/*!
 * @brief Create a thread
 *
 * @param name - the name of the thread
 * @param func - the thread function
 * @param cookie - parameter for the thread function
 * @param startflags - see THREAD_FLAGS_*
 *
 * @return a thread handle for use with other functions, 0 means failure
 *
 * Note: The thread may be running before this thread returns.
 */
intptr_t THREAD_create(const char *name, threadfunc_t *func, intptr_t cookie, int startflags);

/*
 * @brief Kill/Destroy this thread
 * @param h - thread handle
 */
void THREAD_destroy(intptr_t h);

/*!
 * @brief return the thread ID of the current thread
 * @returns thread ID of current thread
 */
intptr_t THREAD_self(void);

/*!
 * @brief Exit the current thread with specified code
 * @param exit_code - the code to exit with
 */
void     THREAD_exit(intptr_t exit_code);

/*!
 * @brief Is this thread alive (running) or has it exited?
 * @param - thread ID
 */
bool     THREAD_isAlive(intptr_t h);

/*!
 * @brief Get the exit code of a thread
 * @param - thread id to get exit code of
 * @return value - the thread exit code if the thread has exited
 */
intptr_t THREAD_getExitValue(intptr_t h);

/*!
 * @brief Get debug name of this thread
 * @param h - thread handle
 * @return a printable string for a thread name
 */
const char *THREAD_name(intptr_t h);

/*!
 * @brief Get debug name of current active thread
 * @return - name of currently active thread
 */
const char *THREAD_selfName(void);

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
