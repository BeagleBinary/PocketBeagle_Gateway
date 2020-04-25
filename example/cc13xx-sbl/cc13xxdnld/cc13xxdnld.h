/******************************************************************************

 @file cc13xxdnld.h

 @brief API for downloading to the CC13/26xx Flash using the ROM
        Bootloader

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/*
//!
//! \defgroup CcDnld
*/

/*
//!
//! \ingroup CcDnld
//@{
*/

//*****************************************************************************
//  @file cc13xxdnld.h
//
//  @brief          API for downloading to the CC13/26xx Flash using the ROM
//                          Bootloader
//
// # Overview #
// The CcDnld API should be used in application code. The CcDnld API is
// intended to ease the integration of the CC13/26xx bootloader functionality
// in a host processor connected to the CC13/26xx UART[Use Cases]
// (@ref USE_CASES).
//
// # General Behavior #
// Before using the CcDnld API:
//   - A binary image should be prepared that is to be loaded into the cc13/26xx
//       Flash.
//  .
// To load data to the CC13/26xx Flash one should:
//   - Set the UART send/recv data callbacks with CcDnld_init().
//   - Connect to the CC13xx/26xx ROM bootloader with CcDnld_connect().
//   - Detect the DeviceId of the target with CcDnld_determineDeviceId().
//   - Optionally Erase Flash with CcDnld_flashEraseRange().
//   - Optionally Program Flash with CcDnld_startDownload() and CcDnld_sendData().
//   - Optionally Erase Flash with CcDnld_flashEraseRange().
//   - Optionally Verify Flash with CcDnld_verify().
//
// # Error handling #
//      The CcDnld API will return CcDnld_Status containing success or error
//      code. The CcDnld_Status codes are:
//      CcDnld_Status_Success
//      CcDnld_Status_Cmd_Error
//      CcDnld_Status_State_Error
//      CcDnld_Status_Param_Error
//   .
//
//
// # Supported Functions #
// | Generic API function          | Description                                                |
// |-------------------------------|------------------------------------------------------------|
// | CcDnld_init()                 | Registers the UART read/write function points              |
// | CcDnld_connect()              | Connects to the CC13xx/26xx ROM bootloader                 |
// | CcDnld_determineDeviceId()    | Determines the deviceId for specific # pages and page size |
// | CcDnld_flashEraseRange()      | Erases the specified flash range                           |
// | CcDnld_startDownload()        | Sends the download command to the CC13xx/26xx ROM          |
// |                               | bootloader                                                 |
// | CcDnld_sendData()             | Sends program data to be program data to be                |
// |                               | programmed in to flash                                     |
// | CcDnld_verify()               | Verify a flash range with data in a buffer                 |
// | CcDnld_getNumPages()          | Utility function to obtain the number of flash pages       |
// | CcDnld_getPageSize()          | Utility function to obtain the page size                   |
//  # Not Supported Functionality #
//
//
//*****************************************************************************
#ifndef CcDnld__include
#define CcDnld__include

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

#include <stdbool.h>
#include <stdint.h>

/// \brief defines the API version
#define CCDNLD_API_VERSION                      "ccDnld-v1.00.00"

/* NOTE: The CCXXX0 devices share the same flash parameters */

/// \brief defines CC13x0/CC13x2/26xx Flash Page Size
#define CCDNLD_CC13X0_PAGE_SIZE                 4096
#define CCDNLD_CC13X2_PAGE_SIZE                 8192

/* NOTE: The CCXXX2 devices share the same flash parameters */

/// \brief defines number CC13x0/CC13x2/26xx Flash Pages
#define CCDNLD_CC13X0_NUM_PAGES                 32
#define CCDNLD_CC13X2_NUM_PAGES                 44

/// \brief defines Max number of bytes that can be transfered in 1 command
#define CCDNLD_MAX_BYTES_PER_TRANSFER           252

/// \brief defines start address
#define CCDNLD_FLASH_START_ADDRESS              0x00000000

/// \brief CcDnld Status and error codes
typedef enum
{
        CcDnld_Status_Success               = 0, ///Success
        CcDnld_Status_Cmd_Error             = 1, ///Command Error
        CcDnld_Status_State_Error           = 2, ///Device State Error
        CcDnld_Status_Param_Error           = 3, ///Parameter Error
        CcDnld_Status_Crc_Error             = 4, ///CRC Error
} CcDnld_Status;

/// \brief UART read write function pointers. These functions are provided by the
///              OS / Platform specific layer to access the CC13xx/26xx UART
typedef struct CcDnld_UartFxns_t {
        uint8_t (*sbl_UartReadByte)(void);
        void (*sbl_UartWriteByte)(uint8_t);
        void (*sbl_UartWrite)(uint8_t*, uint32_t);

} CcDnld_UartFxns_t;

//*****************************************************************************
//
//! \brief Initializes UART function pointer table.
//!
//! This function initializes the UART functions for sending SBL commands to the
//! CC13/26xx
//!
//! \param uartFxns - The UART function pointer table.
//!
//! \return CcDnld_Status
//
//*****************************************************************************
void CcDnld_init(CcDnld_UartFxns_t* uartFxns);

//*****************************************************************************
//
//! \brief Establishes a connection with the ROM Bootloader.
//!
//! This function establishes a connection with the ROM Bootloader by setting
//! the autobaud rate. By sending 0x55 the CC163/26xx ROM bootloader uses the
//! alternating 0's/1's to detect the board rate and sends an Ack/0xCC in
//! response
//!
//! \param None.
//!
//! \return CcDnld_Status
//
//*****************************************************************************
CcDnld_Status CcDnld_connect(void);

//*****************************************************************************
//
//! \brief Determines the deviceId for specific # pages and page size.
//!
//! This function requires an existing connection with the ROM Bootloader by 
//! calling CcDnld_connect(). After the connection has been made successfully 
//! this function can request the deviceId from the ROM Bootloader. This 
//! deviceId can then be used in partnership with the CCDNLD_CCX[0/2]_DEV_ID
//! constants to choose the correct CCDNLD_CC13X[0/2]_PAGE_SIZE and 
//! CCDNLD_CC13X[0/2]_NUM_PAGES in your application logic.
//!
//! \param deviceId - pointer to store the ROM Bootloader's deviceId.
//!
//! \return CcDnld_Status
//
//*****************************************************************************
CcDnld_Status CcDnld_determineDeviceId(uint32_t *deviceId);

//*****************************************************************************
//
//! \brief Gets the number of pages needing to be erased Erases the specified page.
//!
//! This function gets the number of pages needing to be erased based on Flash start address
//! and the Bytes to be written
//!
//! \param startAddress - The Flash start address.
//! \param byteCount    - Number of bytes to be erased.
//! \param deviceType   - String containing the device type [cc13x0, cc13x2, cc26x0, cc26x2]
//!
//! \return CcDnld_Status
//
//*****************************************************************************
CcDnld_Status CcDnld_flashEraseRange(unsigned int startAddress, unsigned int byteCount,
                                                                        char * deviceType);

//*****************************************************************************
//
//! \brief Starts the download to the CC13/26xx ROM bootloader.
//!
//! This function starts the download to the CC13/26xx ROM Bootloader
//!
//! \param startAddress - The Flash start address.
//! \param byteCount    - Total Number of bytes to be programmed.
//! \param deviceType   - String containing the device type [cc13x0, cc13x2, cc26x0, cc26x2]
//!
//! \return CcDnld_Status
//
//*****************************************************************************
CcDnld_Status CcDnld_startDownload(uint32_t startAddress, uint32_t byteCount, char * deviceType);

//*****************************************************************************
//
//! \brief Loads a chunk of data into the CC13/26xx Flash.
//!
//! This function Loads a chunk of data into the CC13/26xx Flash through the
//! ROM bootloader
//!
//! \param startAddress - The Flash start address.
//! \param pData                        - Pointer to a buffer containing the data.
//! \param byteCount        - Number of bytes to be programmed.
//!
//! \return CcDnld_Status
//
//*****************************************************************************
CcDnld_Status CcDnld_sendData(uint8_t *pData, uint32_t byteCount);

//*****************************************************************************
//
//! \brief Loads a chunk of data into the CC13/26xx Flash.
//!
//! This function Loads a chunk of data into the CC13/26xx Flash through the
//! ROM bootloader
//!
//! \param dataAddress  - start address of data in Flash to be verified.
//! \param pData                - Pointer to a buffer containing the data to be
//!                                             verified.
//! \param byteCount        - Number of bytes to be programmed.
//!
//! \return CcDnld_Status
//
//*****************************************************************************
CcDnld_Status CcDnld_verifyData(uint32_t dataAddress, uint8_t *pData,
                                                            uint32_t byteCount);


//*****************************************************************************
//
//! \brief Utility function to obtain the page size based on the targets device ID.
//!
//! \param deviceType  - String containing the device type [cc13x0, cc13x2, cc26x0, cc26x2]
//!
//! \return uint32_t pageSize - The flash page size for the target device Id.
//
//*****************************************************************************
uint32_t CcDnld_getPageSize(char * deviceType);


//*****************************************************************************
//
//! \brief  Utility function to obtain the number of available pages based on the targets device ID.
//!
//! \param deviceType  - String containing the device type [cc13x0, cc13x2, cc26x0, cc26x2]
//!
//! \return uint32_t numPages - The number of available flash pages for the target device Id.
//
//*****************************************************************************
uint32_t CcDnld_getNumPages(char * deviceType);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* CcDnld__include */

//*****************************************************************************
//
//! Close the Doxygen group.
//! @}
//
//*****************************************************************************

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

