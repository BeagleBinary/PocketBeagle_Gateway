/******************************************************************************

 @file main.c

 @brief CC13xx Bootloader Example Application

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#define _GNU_SOURCE /* for strcasestr */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <signal.h>

#include "sblUart.h"
#include "../../cc13xxdnld/cc13xxdnld.h"

#define CONSOLE_COLOR_GREEN     "\033[32m"          /* Green */
#define CONSOLE_COLOR_RED       "\033[31m"          /* Red */
#define CONSOLE_COLOR_YELLOW    "\033[33m"          /* Yellow */
#define CONSOLE_COLOR_RESET     "\033[m"            /* Reset Color */

#define DATA_BUFFER_SIZE            32768 /*32K*/

/* \brief Flash start address for erase, download and verify */
static unsigned int startAddress = 0;
/* \brief Erase flag. If true the CC13xx/26xx Flash will be erased */
static bool eraseFlg = false;
/* \brief Program flag. If true the CC13xx/26xx Flash will be Programed */
static bool programFlg = false;
/* \brief Verify flag. If true the CC13xx/26xx Flash will be Verified */
static bool verifyFlg = false;
/* \brief Binary Mode flag. Set by reading the file extension to decide
          if the download image is binary or Intel Hex format */
static bool binMode = false;
static int hex_lineno;
/* Save port state */
static bool portIsOpen = false;

/* \brief CcDnld_UartFxns_t UART callback functions */
uint8_t uartReadByte(void);
void uartWriteByte(uint8_t byte);
void uartWrite(uint8_t *data, unsigned int len);

static CcDnld_UartFxns_t sblUartFxns = {
    uartReadByte,
    uartWriteByte,
    uartWrite
};

uint8_t uartReadByte(void)
{
    uint8_t readByte;
    SblUart_read(&readByte, 1);
    return readByte;
}

void uartWriteByte(uint8_t byte)
{
    SblUart_write(&byte, 1);
}

void uartWrite(uint8_t *data, unsigned int len)
{
    SblUart_write(data, len);
}

static void usage(char* exeName)
{
    printf("Usage: %s [SERIEL DEVICE] [FILE] [DEVICE TYPE] [OPTION]... \n", exeName);
    printf("-e                                          Erase with optional [START ADDRESS]\n");
    printf("-p                                          Program [FILE] with optional [START ADDRESS]\n");
    printf("-v                                          Verify [FILE] with optional [START ADDRESS]\n");
    printf("--start=[START ADDRESS] Start address for Flash erase, program load and verify\n");
    printf("\nExample: \n%s /dev/ttyS1 FILENAME.BIN [cc13x0, cc13x2, cc26x0 or cc26x2] -e -p -v\n", exeName);
}

static bool convertNumber( const char *txt, unsigned int *num)
{
    char *end_ptr;

   /* accept hex, decimal or octal number format */
   *num = strtol(  txt, &end_ptr, 0 );
    if( (end_ptr==txt) || (*end_ptr != 0) ){
         fprintf(stderr, "Not a number: %s\n", txt );
         return false; /* error */
    }
    else
    {
         return true;
    }
}

void printColor( const char *color,  const char *fmt, ... )
{
    va_list args;
    printf( "%s", color );
    va_start(args,fmt);
    vprintf( fmt, args );
    va_end(args);
    printf( CONSOLE_COLOR_RESET );
}

static void getOptions(int argc, char* argv[])
{
    unsigned int argIdx;
    char *numStart;

    for(argIdx = 1; argIdx < argc; argIdx++)
    {
        if(strcmp(argv[argIdx], "-e") == 0)
        {
            eraseFlg = true;
        }
        else if(strcmp(argv[argIdx], "-p") == 0)
        {
            programFlg = true;
        }
        else if(strcmp(argv[argIdx], "-v") == 0)
        {
            verifyFlg = true;
        }
        else if((numStart = strstr(argv[argIdx], "--start=")) > 0)
        {
            numStart += 8; /* index to end of "--start=" */
            if(!convertNumber(numStart, &startAddress))
            {
                printColor(CONSOLE_COLOR_RED, "Start Address %s not a number. Ignoring --start option\n", numStart);
                startAddress = 0;
            }
            else
            {
                /* The startAddress must still be verified with the verifyStartAddress() function
                        after the deviceId has been detected and we know the proper pageSize
                */
                printf("Start Address: 0x%x\n", startAddress);
            }
        }

    }
}


static void handle_eol_sequence( FILE *fp )
{
    int a,b;

    a = fgetc(fp);
    b = fgetc(fp);
    if( (a=='\r') && (b=='\n') )
    {
        /* MS-DOS format, all is good */
        return;
    }
    if( a == '\r' ){
        /* MAC format, unget B if not eof */
        if( b != EOF ){
            ungetc( b, fp );
        }
        return;
    }
    if( a == '\n' ){
        /* UNIX format unget B if not eof */
        if( b != EOF ){
            ungetc(b,fp);
        }
        return;
    }
    /* something is wrong */
    printColor(CONSOLE_COLOR_RED, "Line: %d bad EOL sequence\n", hex_lineno );
    exit(EXIT_FAILURE);
}

static int getNextIntelHexRecords(unsigned char *pData, unsigned int *startAddr, unsigned int *byteCount, FILE* sblFileHandle)
{
    unsigned char sectorStartAddrHigh, sectorStartAddrLow, sectorSize, recordType;
    unsigned int dataIdx = 0, dataOffset = 0;
    static unsigned short extLinearAddr = 0;
    unsigned char bytesRead;
    int done = 0;
    unsigned int prevSectorEndAddr = -1; /*set it to something that will never happen */
    unsigned int sectorStartAddr;


    *byteCount = 0;

    /* check if it is the start of file and set extLinearAddr to 0 */
    if(ftell(sblFileHandle) == 0)
    {
        extLinearAddr = 0;
    }

    while( (feof(sblFileHandle) == 0) && !done)
    {
        int match;
        int read;
        char pHexStrBuf[1092] = {0};
        unsigned char checksum=0, targetCheckSum;

        hex_lineno++;

        /* read record header */
        read = fread(pHexStrBuf, 1, 9, sblFileHandle);

        if(read != 9)
        {
            printColor(CONSOLE_COLOR_RED, "Error reading file: could not read sector header\n");
            exit(EXIT_FAILURE);
        }

        match = sscanf(pHexStrBuf, ":%02hhx%02hhx%02hhx%02hhx", &sectorSize, &sectorStartAddrHigh, &sectorStartAddrLow, &recordType);
        if(match != 4)
        {
            printColor(CONSOLE_COLOR_RED, "Error reading file: sector header match = %d\n", match);
            printf("line: %d, %s\n", hex_lineno, pHexStrBuf);
            exit(EXIT_FAILURE);
        }

        sectorStartAddr = (sectorStartAddrHigh << 8) | sectorStartAddrLow;

        /*update checksum */
        checksum = sectorSize;
        checksum += sectorStartAddrHigh;
        checksum += sectorStartAddrLow;
        checksum += recordType;

        /*if this is the first sector set the start address */
        if(prevSectorEndAddr == -1)
        {
            /* set the start address and add the extended Linear start address */
            *startAddr = sectorStartAddr | ((extLinearAddr << 16) & 0xFFFF0000);
        }

        /* check if this is he first sector read, or if the next sector is concurrent then read another sector */
        if( ((prevSectorEndAddr == -1) || (prevSectorEndAddr == sectorStartAddr)) &&
             ((dataOffset + sectorSize) < DATA_BUFFER_SIZE) )
        {
            /* check it is a data record */
            if(recordType == 0)
            {
                /*if this is the first sector set the start address */
                if(prevSectorEndAddr == -1)
                {
                    /* set the start address and add the extended Linear start address */
                    *startAddr = sectorStartAddr | (extLinearAddr << 16);
                }

                /* read data, CRC byte and *NOT* the cr/lf */
                bytesRead = fread(pHexStrBuf, 1, (sectorSize+1)*2, sblFileHandle);
                /* now the crlf */
                handle_eol_sequence(sblFileHandle);
                if(bytesRead != ((sectorSize+1)*2))
                {
                    printColor(CONSOLE_COLOR_RED, "Error reading file: bytes read %d\n", bytesRead);
                    printf("%s\n", pHexStrBuf);
                    exit(EXIT_FAILURE);
                }

                for(dataIdx = 0; dataIdx < sectorSize; dataIdx++)
                {
                    /*convert hex string to unsigned char */
                    match = sscanf(&pHexStrBuf[dataIdx*2], "%2hhx", &pData[dataOffset + dataIdx]);
                    if(match != 1)
                    {
                        printColor(CONSOLE_COLOR_RED, "Error reading ihex record: data match = %d dataIdx = %d\n", match, dataIdx);
                        printf("%s\n", pHexStrBuf);
                        exit(EXIT_FAILURE);
                    }
                    *byteCount += 1;
                    checksum += pData[dataOffset + dataIdx];
                }

                dataOffset += dataIdx;

                /* take 2's complement and check checksum */
                sscanf(&pHexStrBuf[dataIdx*2], "%2hhx", &targetCheckSum);
                checksum = -(unsigned int)checksum;
                if(checksum != targetCheckSum)
                {
                    printColor(CONSOLE_COLOR_RED, "Error reading ihex record: checksum mismatch %2hhx:%2hhx\n", checksum, targetCheckSum);
                    printf("%s\n", pHexStrBuf);
                    exit(EXIT_FAILURE);
                }
            }
            else if(recordType == 4)
            {
                unsigned char extLinearAddrHigh, extLinearAddrLow, crc;

                /*set extended linear address */
                bytesRead = fread(pHexStrBuf, 1, (sectorSize+1)*2, sblFileHandle);
                /* now the crlf */
                handle_eol_sequence(sblFileHandle);

                if(bytesRead != (sectorSize+1)*2)
                {
                    printColor(CONSOLE_COLOR_RED, "Error reading file: bytes read %d\n", bytesRead);
                    printf("%s\n", pHexStrBuf);
                    exit(EXIT_FAILURE);
                }

                match = sscanf(pHexStrBuf, "%2hhx%2hhx%2hhx", &extLinearAddrHigh, &extLinearAddrLow, &crc);
                if(match != 3)
                {
                    printColor(CONSOLE_COLOR_RED, "Error reading file: extLinearAddr match = %d\n", match);
                    printf("%s\n", pHexStrBuf);
                    exit(EXIT_FAILURE);
                }
                extLinearAddr = (extLinearAddrHigh << 8) | extLinearAddrLow;

                /* developers can add feature to check CRC */

                *byteCount = 0;
                done = 1;
            }
            else if(recordType == 1)
            {
                /* its the eof record read the remaining bytes */
                while(feof(sblFileHandle) == 0)
                {
                    fread(pHexStrBuf, 1, 1, sblFileHandle);
                }
            }
            else
            {
                /* record type currently not supported */
                printColor(CONSOLE_COLOR_RED, "Error unsupported intel hex record type %x\n", recordType);
                exit(EXIT_FAILURE);
            }

            //set the previous sector end address to check if the next is concurrent
            prevSectorEndAddr = sectorStartAddr + sectorSize;

        }
        else
        {
            /*next sector is not concurrent rewind the file descriptor to start of sector */
            fseek(sblFileHandle, -9, SEEK_CUR);
            done = 1;
        }
    }

    return 0;
}

static void printProgressBar(float percentDone)
{
    uint8_t doneIdx;

    printf("\r[");
    for(doneIdx = 0; doneIdx < 50; doneIdx++)
    {
        if(doneIdx <= (uint8_t)(percentDone/2))
        {
            printf("=");
        }
        else
        {
            printf(" ");
        }
    }
    printf("] %d%% \r", (uint8_t)percentDone);
    fflush(stdout);

    if(percentDone == 100)
    {
         printColor(CONSOLE_COLOR_GREEN, "\r[==================================================] 100%%\n");
    }
}

static void connectToBootloader(void)
{
    printf("Connecting:\n");
    while(CcDnld_connect() != CcDnld_Status_Success)
    {
        char indicator[4] = "|/-\\";
        static uint8_t indicatorIdx = 0;
        printf("\r%c\r", indicator[++indicatorIdx & 0x3]);
        fflush(stdout);
    }
    printColor(CONSOLE_COLOR_GREEN, "\rConnected\n");
}

static void verifyStartAddress(uint32_t pageSize)
{
    /* Verify that the startAddress is at the top of a page in Flash. If it is not at the top of
        a page then it will be ignored and the user will be notified that it has been so
    */
    if((startAddress % pageSize)  != 0)
    {
        printColor(CONSOLE_COLOR_RED, "Start Address 0x%x not on page boundary. Ignoring --start option\n", startAddress);
        startAddress = 0;
    }
}

static void eraseFlash(char * deviceType)
{
    CcDnld_Status ccDnldStatus;
    uint8_t pageIdx;

    uint32_t pageSize = CcDnld_getPageSize(deviceType);
    uint32_t numPages = CcDnld_getNumPages(deviceType);

    printf("Erasing: \n[                                                  ] 00%%");
    fflush(stdout);

    /* set erase page start */
    pageIdx = startAddress / pageSize;

    for(; pageIdx < numPages; pageIdx++)
    {
        float percentDone = 100 * ((float)pageIdx / (float)numPages);
        printProgressBar(percentDone);

        ccDnldStatus = CcDnld_flashEraseRange(pageIdx * pageSize, pageSize, deviceType);
        if(ccDnldStatus != CcDnld_Status_Success)
        {
            printColor(CONSOLE_COLOR_RED, "\n\rFlash erase failed error status %x\n", ccDnldStatus);
            exit(EXIT_FAILURE);
        }
    }
    printProgressBar(100);
}

void downloadFile(FILE* sblFileHandle, unsigned int byteCount, char *deviceType)
{
    CcDnld_Status ccDnldStatus;
    uint8_t pData[DATA_BUFFER_SIZE];
    unsigned int dataIdx = 0;
    unsigned int bytesInTransfer;
    unsigned int byesInBuffer = 0;
    unsigned int bytesLeft;

    printf("Downloading: \n[                                                  ] 00%%");

    if(binMode)
    {
        CcDnld_startDownload(startAddress, byteCount, deviceType);

    }
    else
    {
        /* Intel hex format must read the sectors and call
           CcDnld_startDownload once the sector address and size
           are known */

        /* For intel hex the num bytes is roughly 1/2 the bytes
           in the file as the bytes are ascii. This is ignoring
           the Sector headers */
        byteCount /= 2;
    }

    /* set bytesLeft so we know the progress */
    bytesLeft = byteCount;

    /* make sure the file pointer is at the start of the file */
    rewind(sblFileHandle);

    while(feof(sblFileHandle) == 0)
    {
        float percentDone = 100 - (100 * ((float)bytesLeft / (float)byteCount));
        printProgressBar(percentDone);

        /* check if we need to read more data */
        if(byesInBuffer == 0)
        {
            dataIdx = 0;
            if(binMode)
            {
                byesInBuffer = fread(pData, 1, DATA_BUFFER_SIZE, sblFileHandle);
            }
            else
            {
                unsigned int sectorStartAddress;
                /* get next intel hex records */
                getNextIntelHexRecords(pData, &sectorStartAddress, &byesInBuffer, sblFileHandle);
                if( CcDnld_startDownload(sectorStartAddress, byesInBuffer, deviceType) != CcDnld_Status_Success )
                {
                    printColor(CONSOLE_COLOR_RED, "\n\rError during download\n");
                    fclose(sblFileHandle);
                    exit(EXIT_FAILURE);
                }
            }
        }

        /* Limit transfer count */
        if(CCDNLD_MAX_BYTES_PER_TRANSFER < byesInBuffer)
        {
            bytesInTransfer = CCDNLD_MAX_BYTES_PER_TRANSFER;
        }
        else
        {
            bytesInTransfer = byesInBuffer;
        }

        /* Send Data command */
        ccDnldStatus = CcDnld_sendData(&pData[dataIdx], bytesInTransfer);
        if(ccDnldStatus != CcDnld_Status_Success)
        {
            unsigned int retry = 0;
            /* Allow up to 3 successive retries to send the data to the target */
            while(retry++ < 3 && (ccDnldStatus = CcDnld_sendData(&pData[dataIdx], bytesInTransfer)) != CcDnld_Status_Success);
            if(ccDnldStatus != CcDnld_Status_Success)
            {
                printColor(CONSOLE_COLOR_RED, "\n\rError during download\n");
                fclose(sblFileHandle);
                exit(EXIT_FAILURE);
            }
        }

        /* Update index and bytesLeft */
        byesInBuffer -= bytesInTransfer;
        dataIdx += bytesInTransfer;
        bytesLeft -= bytesInTransfer;
    }

    printProgressBar(100);
}

void verifyFile(FILE* sblFileHandle, unsigned int byteCount)
{
    CcDnld_Status ccDnldStatus;
    uint8_t pData[DATA_BUFFER_SIZE];
    unsigned int byesInBuffer = 0;
    unsigned int bytesLeft;

    printf("Verifying: \n[                                                  ] 00%%");

    /* make sure the file pointer is at the start of the file */
    rewind(sblFileHandle);

    bytesLeft = byteCount;
    unsigned int dataOffset = startAddress;
    while(feof(sblFileHandle) == 0)
    {
        float percentDone = 100 - (100 * ((float)bytesLeft / (float)byteCount));
        printProgressBar(percentDone);

        /* read more data */
        if(binMode)
        {
            byesInBuffer = fread(pData, 1, DATA_BUFFER_SIZE, sblFileHandle);
        }
        else
        {
            /* get next intel hex records */
            getNextIntelHexRecords(pData, &dataOffset, &byesInBuffer, sblFileHandle);
        }

        ccDnldStatus = CcDnld_verifyData(dataOffset, pData, byesInBuffer);
        if(ccDnldStatus == CcDnld_Status_Crc_Error)
        {
            printColor(CONSOLE_COLOR_RED, "CRC error\n");
            fclose(sblFileHandle);
            exit(EXIT_FAILURE);
        }
        else if(ccDnldStatus != CcDnld_Status_Success)
        {
            printColor(CONSOLE_COLOR_RED, "Error during verify\n");
            fclose(sblFileHandle);
            exit(EXIT_FAILURE);
        }

        /* Update bytesLeft and data offset */
        bytesLeft -= byesInBuffer;
        dataOffset += byesInBuffer;
    }

    printProgressBar(100);
}

static void exitHandler(void)
{
    if(portIsOpen && SblUart_close())
    {
        printf("Abnormal termination, port close successful\n");
    }
}

static void sigHandler(int sig)
{
    printf("Signal: %d\n", sig);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    FILE* sblFileHandle;
    char * selectedSerialPort;
    char * binFile;
    char * deviceType;
    struct stat st;
    unsigned int byteCount;
    uint32_t pageSize;

#ifdef SBL_TWO_WIRE
    printf("Using Two wrire SBL Interface\n");
#else
    printf("Using USB SBL Interface\n");
#endif

    /* Setup the exit handler */
    atexit(exitHandler);

    signal(SIGHUP,  sigHandler); /* Hangup */
    signal(SIGINT,  sigHandler); /* Ctrl-C */
    signal(SIGILL,  sigHandler); /* Illegal instruction */
    signal(SIGABRT, sigHandler); /* Abort */
    signal(SIGSEGV, sigHandler); /* Segment fault */
    signal(SIGTERM, sigHandler); /* Terminate */
    signal(SIGSTOP, sigHandler); /* Stop */

    printf("%s %s -- %s %s\n\n", argv[0], CCDNLD_API_VERSION, __DATE__, __TIME__);

    if (argc < 4)
    {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    else
    {
        selectedSerialPort = argv[1];
        binFile = argv[2];
        deviceType = argv[3];
    }

    getOptions(argc, argv);

    if (CcDnld_getPageSize(deviceType) == 0) {
        printColor(CONSOLE_COLOR_RED, "Invalid device type %s\n", deviceType);
        exit(EXIT_FAILURE);
    }
    else {
        printColor(CONSOLE_COLOR_GREEN, "\rDevice set to: %s\n", deviceType);
    }

    printf("Opening serial port %s\n", selectedSerialPort);
    if (SblUart_open(selectedSerialPort) == -1)
    {
        printColor(CONSOLE_COLOR_RED, "could not open serial port\n");
        exit(EXIT_FAILURE);
    }

    portIsOpen = true;

    /* Check if file is a Binary or Intel Hex */
    if(strcasestr(binFile, ".bin") != 0)
    {
        binMode = true;
    }
    else if(strcasestr(binFile, ".hex") != 0)
    {
        binMode = false;
    }
    else
    {
        printColor(CONSOLE_COLOR_YELLOW, "Unrecognized file extension. Supported formats are *.bin and *.hex\n");
        printColor(CONSOLE_COLOR_YELLOW, "Assuming Intel Hex format\n");
        binMode = false;
    }

    printf("Open binary file %s\n", binFile);
    sblFileHandle = fopen(binFile, "r");

    if(sblFileHandle == NULL)
    {
        printColor(CONSOLE_COLOR_RED, "Binary file cannot be read\n");
        exit(EXIT_FAILURE);
    }

    if( stat( binFile, &st ) != 0 ){
        perror( binFile );
        printColor(CONSOLE_COLOR_RED, "Error reading size of Binary file\n");
        exit(EXIT_FAILURE);
    }

    byteCount = st.st_size;
    if(byteCount==0)
    {
        printColor(CONSOLE_COLOR_RED, "Binary file 0 length\n");
        exit(EXIT_FAILURE);
    }
    printf("Binary file size = %d\n", byteCount);

    CcDnld_init(&sblUartFxns);

    /* connect to CC13xx bootloader */
    connectToBootloader();

    /* Now that we know the device type, we can verify the startAddress is valid */
    if(startAddress)
    {
        pageSize = CcDnld_getPageSize(deviceType);

        verifyStartAddress(pageSize);
    }

    if(eraseFlg)
    {
        eraseFlash(deviceType);
    }

    if(programFlg)
    {
        downloadFile(sblFileHandle, byteCount, deviceType);
    }

    if(verifyFlg)
    {
        verifyFile(sblFileHandle, byteCount);
    }

    printColor(CONSOLE_COLOR_GREEN, "\n\rOperation completed successfully\n");

    if(SblUart_close())
    {
        portIsOpen = false;
    }

    fclose(sblFileHandle);

    return (0);
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
