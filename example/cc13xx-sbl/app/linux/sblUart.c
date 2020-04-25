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

/*********************************************************************
 * INCLUDES
 */
#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static int serialPortFd;
static struct termios tioOld;
/*********************************************************************
 * API FUNCTIONS
 */

int SblUart_open(const char *devicePath)
{
    struct termios tio;
    int rtn;

    /* open the device */
    serialPortFd = open(devicePath, O_RDWR | O_NOCTTY);
    if (serialPortFd < 0)
    {
        perror(devicePath);
        printf("sblUartOpen: %s open failed\n", devicePath);
        return (-1);
    }

    rtn = tcgetattr(serialPortFd, &tioOld);
    if(rtn == -1)
    {
        printf("sblUartOpen: tcgetattr error: %d\n", rtn);
        return (-1);
    }

    /* set Baud rate */
    rtn = cfsetspeed(&tio, B1500000);
    if(rtn == -1)
    {
        printf("sblUartOpen: cfsetspeed error: %d\n", rtn);
        return (-1);
    }

    /* Set raw mode:
        tio->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                        | INLCR | IGNCR | ICRNL | IXON);
        tio->c_oflag &= ~OPOST;
        tio->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
        tio->c_cflag &= ~(CSIZE | PARENB);
        tio->c_cflag |= CS8;
    */
    cfmakeraw(&tio);

    /* Specifically: 1 stop bit!
     *
     * the CC1310 boot loader uses exactly 1 stop bit.
     * And will transmit bytes back to back with ZERO
     * delay between bytes. The only delay is exactly
     * the stopbit count.
     *
     * The USB-ACM (usb-serial) uart on the launchpad
     * will reject these bytes because they are not
     * properly framed. Other serial interfaces will
     * accept the byte (for example an FTDI chip)
     *
     * The result: the bootloader cannot connect.
     */
    tio.c_cflag &= (~CSTOPB);

#ifdef SBL_TWO_WIRE
    tio.c_iflag &= ~(IXOFF);
    tio.c_cflag |= (CREAD | CLOCAL);
#endif

    /* Make it block for 200ms */
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 2;

    tcflush(serialPortFd, TCIFLUSH);
    rtn = tcsetattr(serialPortFd, TCSANOW, (const struct termios *) (&tio));
    if(rtn == -1)
    {
        printf("sblUartOpen: tcsetattr error: %d\n", rtn);
        return (-1);
    }

    return 0;
}

bool SblUart_close(void)
{
    int rtn;
    bool ret = true;

    tcflush(serialPortFd, TCOFLUSH);

    rtn = tcsetattr(serialPortFd, TCSANOW, (const struct termios *) (&tioOld));
    if(rtn == -1)
    {
        printf("SblUart_close: tcsetattr error: %d %s\n", rtn, strerror(errno));
        ret = false;
    }
    else
    {
        /* Verify we can close the port */
        rtn = close(serialPortFd);
        if(rtn == -1)
        {
            printf("SblUart_close: Port close failure: %d %s\n", rtn, strerror(errno));
            ret = false;
        }
    }

    return (ret);
}

void SblUart_write(const unsigned char* buf, size_t len)
{
#ifdef SBL_TWO_WIRE
  if(write(serialPortFd, buf, len) != len)
  {
    printf("Write failed... Flushing.\n");
    tcflush(serialPortFd, TCOFLUSH);
    write (serialPortFd, buf, len);
  }
#else
    write (serialPortFd, buf, len);
    tcflush(serialPortFd, TCOFLUSH);
#endif
    return;
}

unsigned char SblUart_read(unsigned char* buf, size_t len)
{
    unsigned char ret = read(serialPortFd, buf, len);

    return (ret);
}
