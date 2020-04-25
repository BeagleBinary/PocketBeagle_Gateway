/******************************************************************************
 @file rand_data.c

 @brief TIMAC 2.0 API Random data generator implimentation

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#include "compiler.h"
#include "rand_data.h"
#include <string.h>

/*
 * Initialize a single random data source with a seed.
 *
 * Public function defined in rand_data.h
 */
void RAND_DATA_initOne(struct rand_data_one *pRD1, uint32_t seed)
{
    /* rule: always zero in case we add to the struct later */
    memset((void *)(pRD1), 0, sizeof(*pRD1));
    pRD1->cnt  = 0;
    pRD1->next = seed;
}

/*
 * Initialize a pair of random data source with a seed.
 *
 * Public function defined in rand_data.h
 */
void RAND_DATA_initPair(struct rand_data_pair *pRD, uint32_t seed)
{
    /* rule: always zero in case we add to the struct later */
    memset((void *)(pRD), 0, sizeof(*pRD));
    RAND_DATA_initOne(&(pRD->tx), seed);
    RAND_DATA_initOne(&(pRD->rx), seed);
}

/*
 * Get the next byte from a single random data source
 *
 * Public function defined in rand_data.h
 */
uint8_t RAND_DATA_nextByte(struct rand_data_one *pRD1)
{
    /* see: https://en.wikipedia.org/wiki/Linear_congruential_generator */
    /* see: http://stackoverflow.com/questions/8569113/why-1103515245-is-used-in-rand */
    uint32_t r;
    pRD1->cnt += 1;
    pRD1->next = (pRD1->next * 1103515245) + 12345;
    r = pRD1->next;
    r = r >> 16;
    r = r & 0x0ff;
    return ((uint8_t)(r));
}

/*
 * Get the next byte from the tx side of a random data generator.
 *
 * Public function defined in rand_data.h
 */
uint8_t RAND_DATA_nextTx(struct rand_data_pair *pRDP)
{
    /* call for specific tx */
    return (RAND_DATA_nextByte(&(pRDP->tx)));
}

/*
 * Get the next byte from the rx side of a random data generator.
 *
 * Public function defined in rand_data.h
 */
uint8_t RAND_DATA_nextRx(struct rand_data_pair *pRDP)
{
    /* call for specific rx */
    return (RAND_DATA_nextByte(&(pRDP->rx)));
}

/*
 * generate a buffer full of random data from a genrator.
 *
 * Public function defined in rand_data.h
 */
void    RAND_DATA_generateBuf(struct rand_data_one *pRD1,
                               uint8_t *pBuf, size_t n)
{
    /* loop over.. generating */
    while(n>0)
    {
        *pBuf = RAND_DATA_nextByte(pRD1);
        pBuf++;
        n--;
    }
}

/*
 * Verify a buffer against a random data generator
 *
 * Public function defined in rand_data.h
 */
int     RAND_DATA_verifyBuf(struct rand_data_one *pRD1,
                             const uint8_t *pBuf, size_t n)
{
    uint8_t cv;
    size_t  x;

    /* use a for loop, we need to return the index on error */
    for(x = 0 ; x < n ; x++)
    {
        cv = RAND_DATA_nextByte(pRD1);
        if(pBuf[x] != cv)
        {
            /* We use (x+1) in case (x==0) */
            return -((int)(x+1));
        }
    }
    /* match! */
    return (0);
}

/*
 * Generate test tx data from a pair
 *
 * Public function defined in rand_data.h
 */
void RAND_DATA_txGenerate(struct rand_data_pair *pRDP,
                           uint8_t *pBuf, size_t n)
{
    /* call the generator */
    RAND_DATA_generateBuf(&(pRDP->tx), pBuf, n);
}

/*
 * Verify rx data from a pair
 *
 * Public function defined in rand_data.h
 */
int  RAND_DATA_rxVerify(struct rand_data_pair *pRDP, uint8_t *pBuf, size_t n)
{
    /* call the verifier */
    return (RAND_DATA_verifyBuf(&(pRDP->rx), pBuf, n));
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
