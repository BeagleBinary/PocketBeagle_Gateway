/******************************************************************************
 @file ti_semaphore.c

 @brief TIMAC 2.0 API Implimentation of the semaphore abstraction

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#include "compiler.h"
#include "ti_semaphore.h"
#include "log.h"
#include "hlos_specific.h"

#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

  /*
   * @var sem_check
   * @brief [private] Used to validate a semaphore pointer.
   */
static const int sem_check = 'S';
/*
 * @var _sem_no_name
 * @brief default name for a semaphore with no name
 */
static const char _sem_no_name[] = "sem-no-name";

/* @struct semaphore_details
 * @brief abstract semaphore
 */
struct semaphore_details {
    const int *check_ptr;
    const char *dbg_name;
    intptr_t s;
};

/*
 * @brief convert a handle to a semaphore pointer
 * @param h - handle
 * @returns valid handle or null
 */
static struct semaphore_details *h2pS(intptr_t h)
{
    struct semaphore_details *pS;

    pS = (struct semaphore_details *)(h);
    if(pS)
    {
        if(pS->check_ptr != &sem_check)
        {
            pS = NULL;
            BUG_HERE("not a semaphore: %p\n", (void *)(pS));
        }
    }
    return (pS);
}

/*
 * Destroy a semaphore
 *
 * Public function defined in ti_semaphore.h
 */
void SEMAPHORE_destroy(intptr_t h)
{
    struct semaphore_details *pS;

    pS = h2pS(h);
    if(pS == NULL)
    {
        return;
    }

    if(pS->s)
    {
        _ATOMIC_sem_destroy(pS->s);
        pS->s = 0;
    }

    if(pS->dbg_name)
    {
        free_const((const void *)(pS->dbg_name));
        pS->dbg_name = NULL;
    }
    memset((void *)(pS), 0, sizeof(*pS));
    free((void *)(pS));
}

/*
 * Create a semaphore
 *
 * Public function defined in ti_semaphore.h
 */
intptr_t SEMAPHORE_create(const char *dbg_name, unsigned initial_value)
{
    struct semaphore_details *pS;

    /* get space */
    pS = calloc(1, sizeof(*pS));
    if(pS == NULL)
    {
        LOG_printf(LOG_ERROR, "sem: no memory\n");
        return (0);
    }

    /* get a default name */
    if(dbg_name == NULL)
    {
        dbg_name = _sem_no_name;
    }

    /* mark struct as valid. */
    pS->check_ptr = &sem_check;
    pS->dbg_name = strdup(dbg_name);
    if(pS->dbg_name == NULL)
    {
        BUG_HERE("no memory\n");
    }

    pS->s = _ATOMIC_sem_create();
    if(pS->s == 0)
    {
        BUG_HERE("cannot create semaphore\n");
    }
    while(initial_value > 0)
    {
        initial_value--;
        _ATOMIC_sem_put(pS->s);
    }
    return (intptr_t)(pS);
}

/*
 * Put to a semaphore N times
 *
 * Public function defined in ti_semaphore.h
 */
int SEMAPHORE_putN(intptr_t h, unsigned n)
{
    struct semaphore_details *pS;

    pS = h2pS(h);
    if(pS == NULL)
    {
        return (-1);
    }
    else
    {
        while(n > 0)
        {
            n--;
            _ATOMIC_sem_put(pS->s);
        }
        return (0);
    }
}

/*
 * Wait on a seamphore N times
 *
 * Public function defined in ti_semaphore.h
 */
int SEMAPHORE_waitNWithTimeout(intptr_t h, unsigned n, int timeout_mSec)
{
    struct semaphore_details *pS;
    int r;

    pS = h2pS(h);
    if(pS == NULL)
    {
        return (-1);
    }

    if(((timeout_mSec == 0) && (n != 1)))
    {
        BUG_HERE("sem operation not supported\n");
    }

    r = 1;
    while(n > 0)
    {
        n--;
        r = _ATOMIC_sem_get(pS->s, timeout_mSec);
    }
    return (r);
}

/*
 * Inspect (return current count) a semaphore
 *
 * Public function defined in ti_semaphore.h
 */
int SEMAPHORE_inspect(intptr_t h)
{
    struct semaphore_details *pS;
    int r;

    pS = h2pS(h);
    if(pS == NULL)
    {
        return (-1);
    }

    r = _ATOMIC_sem_cnt(pS->s);
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

