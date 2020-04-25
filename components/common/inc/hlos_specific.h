/******************************************************************************
 @file hlos_specific.h

 @brief TIMAC 2.0 API Prototypes for Highlevel OS specific functions. (HLOS = Windows, Linux, etc)

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#if !defined(HLOS_SPECIFIC_H)
#define HLOS_SPECIFIC_H

/*
 * internal generic functions
 * used by the generic layer to do things
 * with a specific os
 */

/*
 * @brief Exit, with some fatal error code.
 * @param the exit code
 */
void MSVC_NO_RETURN _FATAL_exit(int code) GCC_NO_RETURN;

/*
 * @brief Exit, main use case is MS Windows apps.
 *
 * This a message and waits for the user to press enter.
 * On windows, this is important otherwise on windows
 * the console application sort of "flashes" on screen.
 */
void MSVC_NO_RETURN _EXIT_pause(int code) GCC_NO_RETURN;

/*
 * @brief  Lock the world, globally and atomically.
 */
void     _ATOMIC_global_lock(void);

/*
 * @brief Unlock the world, globally and atomically.
 */
void     _ATOMIC_global_unlock(void);

/*
 * @brief Create an atomic (mutex) type lock
 */
intptr_t _ATOMIC_local_create(void);

/*
 * @brief Destroy an atomic (mutex) type lock
 */
void     _ATOMIC_local_destroy(intptr_t h);

/*
 * @brief Lock an atomic item (take exclusive ownersship of)
 * @param h - the atomic object
 * @param timeout_mSecs -1=forever, 0=non-blocking, >0 mSec timeout
 * This supports recursive locks.
 */
int      _ATOMIC_local_lock(intptr_t h, int timeout_mSecs);

/* @brief Unlock the atomic item (release the mutex)
 * Note: the unlocking thread must be the locking thread.
 */
void     _ATOMIC_local_unlock(intptr_t h);

/*
 * @brief Create a counting semaphore.
 */
intptr_t _ATOMIC_sem_create(void);

/*
 * @brief Destroy a counting semaphore.
 */
void _ATOMIC_sem_destroy(intptr_t h);

/*
 * @brief the current count value of a semaphore
 */
int _ATOMIC_sem_cnt(intptr_t h);

/*
 * @brief Subtract/Get/Consume from the semaphore
 * @param h - semaphore
 * @param timeout_mSecs -1=forever, 0=non-blocking, >0 mSec timeout
 */
int _ATOMIC_sem_get(intptr_t h, int timeout_mSecs);

/*
 * @brief Add/Put/Produce to a seamphore
 */
void _ATOMIC_sem_put(intptr_t h);

/*
 * @brief Get the current absolute wall clock time.
 */
uint64_t _TIMER_getAbsNow(void);

/*
 * @brief Sleep for n Milliseconds
 */
void _TIMER_sleep(uint32_t mSecs);

/*
 * @brief Return the current thread id.
 */
intptr_t _THREAD_self(void);

/*
 * @brief Create a thread
 * @param thread_func - the thread code
 * @param param - a parameter for the thread
 * Note: the thread runs when created.
 */
intptr_t _THREAD_create(const char *dbg_name,
                        intptr_t (*thread_func)(intptr_t threadparam),
                         intptr_t param);

/*
 * @brief The thread wants to exit instead of returning.
 */
void     _THREAD_exit(void);

/*
 * @brief Destroy a thread.
 */
void     _THREAD_destroy(intptr_t os_token);

/*
 * @brief Make the console beep
 */
void _DEBUG_beep(void);

/*
 * @brief disable keyboard key as you type
 */
void _DEBUG_echo_off(void);

/*
 * @brief re-enable keyboard echo as you type
 */
void _DEBUG_echo_on(void);

/*
 * @brief Poll the keyboard if no key is pressed return EOF otherwise the key
 *
 * see debug_helpers.h for definition of keys like F1, PgUp, etc
 */
int _DEBUG_getkey(void);

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

