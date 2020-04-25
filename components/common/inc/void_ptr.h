/******************************************************************************
 @file void_ptr.h

 @brief TIMAC 2.0 API Compiler macros to handle void pointers

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#if !defined(VOID_PTR_H)
#define VOID_PTR_H

/*!
 *
 * OVERVIEW
 * ========
 *
 * Some compilers complain when code performs "void pointer math"
 * Other compilers treat a void pointer as a "uint8_t" byte pointer.
 *
 * These macros help elimiinate compiler warning noise.
 */

/*!
 * @def c_void_ptr_add() add a constant to a 'const' void pointer.
 *
 * @param VP - the void pointer
 * @param N  - number (of bytes) to add to the void pointer
 * @return A new void pointer
 */
#define c_void_ptr_add(VP, N)   ((const void *)(((const char *)(VP))+(N)))

/*!
 * @def void_ptr_add() add a constant to a 'non-const' void pointer.
 *
 * @param VP - the void pointer
 * @param N  - number (of bytes) to add to the void pointer
 * @return A new void pointer
 */
#define void_ptr_add(VP, N)     ((void *)(((char *)(VP))+(N)))

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

