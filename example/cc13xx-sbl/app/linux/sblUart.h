/******************************************************************************

 @file sblUart.c

 @brief CC13xx Bootloader Platform Specific UART functions

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef SBL_UART_H
#define SBL_UART_H

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>


//*****************************************************************************
//
//! \brief Opens the UART to the CC13/26xx ROM bootloader.
//!
//! This functions opens the UART to the CC13/26xx ROM bootloader.
//!
//! \param devicePath - path to the UART device.
//!
//! \return returns 0 for success and -1 for error
//
//*****************************************************************************
int SblUart_open(const char *devicePath);

//*****************************************************************************
//
//! \brief Closes the UART to the CC13/26xx ROM bootloader.
//!
//! This functions closes the UART to the CC13/26xx ROM bootloader.
//!
//! \param none.
//!
//! \return true if the port was successfully closed, false otherwise.
//
//*****************************************************************************
bool SblUart_close(void);

//*****************************************************************************
//
//! \brief Writes to the CC13/26xx ROM bootloader UART.
//!
//! This functions writes to the CC13/26xx ROM bootloader UART.
//!
//! \param buf - pointer to a buffer of byte to be written
//! \param len - length of the buffer to be written
//!
//! \return none
//
//*****************************************************************************
void SblUart_write(const unsigned char* buf, size_t len);

//*****************************************************************************
//
//! \brief Reads from the CC13/26xx ROM bootloader UART.
//!
//! This functions reads from the CC13/26xx ROM bootloader UART.
//!
//! \param buf - pointer to a buffer to be read in to
//! \param len - length of the buffer
//!
//! \return returns number of bytes read
//
//*****************************************************************************
unsigned char SblUart_read(unsigned char* buf, size_t len);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //SBL_UART_H
