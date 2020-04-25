/******************************************************************************
 @file app_main.c

 @brief TIMAC 2.0 API Primary application file for NPI server application

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#include "compiler.h"
#include "npi_server2.h"
#include "threads.h"
#include "timer.h"
#include "stream.h"
#include "stream_socket.h"
#include "stream_uart.h"
#include "fatal.h"
#include "log.h"
#include "mutex.h"
#include <string.h>
#include <malloc.h>

#include "stream.h"

struct uart_cfg my_uart_cfg;
struct socket_cfg my_socket_cfg;
struct mt_msg_interface common_uart_interface;
static intptr_t uart_thread_id;
static bool     uart_thread_ready;
static intptr_t server_thread_id;
static bool     server_thread_ready;
static intptr_t uart_mutex;

struct mt_msg_interface socket_interface_template;

struct npi_connection {
    /* has something gone wrong this is set to true */
    bool     is_dead;
    bool     u2s_busy;
    bool     s2u_busy;
    char     *dbg_name;

    /* set to true when the socket side is ready to proceed */
    bool     socket_ready;
    /* set to true when the uart side is ready to proceed */
    bool     uart_ready;
    /* what connection number is this? */
    int  connection_id;

    /* uart side "areq" items get put here */
    struct mt_msg_list areq_list;

    struct mt_msg_interface socket_interface;
    intptr_t thread_id_u2s;
    intptr_t thread_id_s2u;
    struct npi_connection *pNext;
};

static struct npi_connection *all_connections;
static intptr_t all_connections_mutex;

static void lock_connection_list(void)
{
    MUTEX_lock(all_connections_mutex, -1);
}

static void unlock_connection_list(void)
{
    MUTEX_unLock(all_connections_mutex);
}

/*
 * Lock the uart so that only a single connection can access.
 * This is a blocking call
 *
 * the example NPI server supports multiple "upstream-side" (socket)
 * connections, in the case when two clients {sockets} are connected.
 * If socket (A) is sending an SREQ to the device, we must wait for
 * the SRSP from that device before allowing socket (B) to proceed.
 */
static void lock_uart(void)
{
    int r;
    int n;
    n = 0;
    for(;;)
    {
        r = MUTEX_lock(uart_mutex, 1000);
        if(r == 0)
        {
            break;
        }
        n++;
        if(n < 5)
        {
            continue;
        }
        LOG_printf(LOG_ERROR,"UART locked %d seconds by: %s\n",
                   n, MUTEX_lockerName(uart_mutex));
    }

}

/*
 * See lock_uart()
 */
static void unlock_uart(void)
{
    MUTEX_unLock(uart_mutex);
}

void APP_defaults(void)
{
    my_uart_cfg.devname = "/dev/ttyACM0";
    my_uart_cfg.baudrate = 115200;
    my_uart_cfg.open_flags = STREAM_UART_FLAG_rd_thread;

    my_socket_cfg.ascp = 's';
    // do not bind to a specific ip address
    my_socket_cfg.host = NULL; /* strdup("localhost"); */
    my_socket_cfg.server_backlog = 1;
    my_socket_cfg.device_binding = NULL;
    my_socket_cfg.service = strdup("5000");
    if(my_socket_cfg.service == NULL)
    {
        BUG_HERE("No memory\n");
    }

    common_uart_interface.dbg_name = strdup("uart");
    common_uart_interface.frame_sync = true;
    common_uart_interface.include_chksum = true;
    common_uart_interface.s_cfg = NULL;
    common_uart_interface.u_cfg = &my_uart_cfg;
    common_uart_interface.rx_thread = 0;
    common_uart_interface.tx_frag_size = (
        256 - /* max message */
        1 - /* sync byte */
        1 - /* length */
        1 - /* cmd0 */
        1 - /* cmd1 */
        1 - /* Extended header VERSION/STACK */
        2 - /* total size */
        1 - /* block number */
        /* data */
        1); /* checksum */
    common_uart_interface.retry_max = 3;
    common_uart_interface.frag_timeout_mSecs = 2000;
    common_uart_interface.srsp_timeout_mSecs = 2000;
    common_uart_interface.stack_id = 0;
    common_uart_interface.len_2bytes = false;
    common_uart_interface.rx_handler_cookie = false;
    /* frag_ack - handled by init */
    /* srsp_list - handled by init */
    /* other_list - handled by init */
    common_uart_interface.is_dead = false;
    common_uart_interface.flush_timeout_mSecs = 50;
    common_uart_interface.intermsg_timeout_mSecs = 3000;

    socket_interface_template.is_NPI   = true;
    socket_interface_template.dbg_name = strdup("socket");
    if(socket_interface_template.dbg_name == NULL)
    {
        BUG_HERE("No memory\n");
    }
    socket_interface_template.frame_sync = true;
    socket_interface_template.include_chksum = true;
    socket_interface_template.s_cfg = NULL;
    socket_interface_template.u_cfg = NULL;
    socket_interface_template.rx_thread = 0;
    /* frag size does not matter, we do 2 byte lengths. */
    socket_interface_template.tx_frag_size = 3000;
    socket_interface_template.retry_max = 3;
    socket_interface_template.frag_timeout_mSecs = 2000;
    socket_interface_template.srsp_timeout_mSecs = 2000;
    socket_interface_template.stack_id = 0;
    socket_interface_template.len_2bytes = true;
    socket_interface_template.rx_handler_cookie = false;
    /* frag_ack - handled by init */
    /* srsp_list - handled by init */
    /* other_list - handled by init */
    socket_interface_template.is_dead = false;
    socket_interface_template.flush_timeout_mSecs = 50;
    socket_interface_template.intermsg_timeout_mSecs = 3000;

}

/*!
 * @brief Transmit a message to the uart, rx reply if required
 * @param pMsg  the message to transmit
 * @returns number of messages transferred.
 */
static int uart_txrx(struct mt_msg *pMsg)
{
    int r;

    lock_uart();

    r = MT_MSG_txrx(pMsg);

    unlock_uart();

    return r;
}

/*!
 * @brief This thread function transfers AREQ messages from the uart to the socket.
 * @param cookie - thread parameter
 * @returns nothing
 */
static intptr_t u2s_thread(intptr_t cookie)
{
    struct npi_connection *pCONN;
    struct mt_msg *pMsg;

    pCONN = (struct npi_connection *)(cookie);
    pCONN->uart_ready = true;

    /* it should be ready *QUICKLY* */
    while(!(pCONN->socket_ready))
    {
        TIMER_sleep(10);
    }

    pCONN->u2s_busy = true;
    for(;;)
    {
        /* is our uart dead? */
        /*  since it is a USB device it might go away.. */
        if(STREAM_isError(common_uart_interface.hndl))
        {
            /* We are dead */
            pCONN->is_dead = true;
        }

        /* The socket thread might also set dead. */
        if(pCONN->is_dead)
        {
            break;
        }

        /* wait for an AREQ to come in */
        pMsg = MT_MSG_LIST_remove(&(pCONN->socket_interface),
                                   &(pCONN->areq_list), 5000);
        if( pCONN->is_dead ){
            continue;
        }
        if(pMsg == NULL)
        {
            /* no areq.. timeout.. */
            continue;
        }

        /* we just transmit it to the socket */
        MT_MSG_setDestIface(pMsg, &(pCONN->socket_interface));

        /* but first, we must reformat for the socket */
        MT_MSG_reformat(pMsg);

        /* we just send it */
        MT_MSG_txrx(pMsg);

        MT_MSG_log(LOG_DBG_MT_MSG_traffic, pMsg, "*** Sending UART Data to "
            "socket (%s -> %s). Sequence ID: %d Length: %d\n",
            pMsg->pSrcIface->dbg_name, pMsg->pDestIface->dbg_name,
            pMsg->sequence_id, pMsg->expected_len);

        /* we don't need this any more */
        MT_MSG_free(pMsg);
    }
    pCONN->u2s_busy = false;

    /* wait for the socket to uart thread to die */
    while(THREAD_isAlive(pCONN->thread_id_s2u))
    {
        TIMER_sleep(1000);
    }
    lock_connection_list();
    /* remove this connection from the list. */
    {
        struct npi_connection **ppCON;
        ppCON = &all_connections;

        /* Hunt the list.. */
        while(*ppCON)
        {
            if(*ppCON != pCONN)
            {
                /* not this one */
                ppCON = &((*(ppCON))->pNext);
                continue;
            }
            else
            {
                /* found it */
                break;
            }
        }

        /* found it */
        if(*ppCON)
        {
            /* remove from the list */
            *ppCON = pCONN->pNext;
            pCONN->pNext = NULL;
        }
        else
        {
            /* this should never happen! */
        }
    }
    unlock_connection_list();

    /* Now we can release the pCONN */
    free((void *)(pCONN));
    return 0;
}

/* log message we are forwarding */
static void say_forward(struct mt_msg *pMsg)
{
    const char *cp;

    cp = "UNKNOWN";

    switch (pMsg->m_type)
    {
    default:
        cp = "other";
        break;
    case MT_MSG_TYPE_areq:
        cp = "AREQ";
        break;
    case MT_MSG_TYPE_sreq:
        cp = "SREQ";
        break;
    case MT_MSG_TYPE_poll:
        cp = "POLL";
        break;
    case MT_MSG_TYPE_srsp:
        cp = "SRSP";
        break;
    }

    MT_MSG_log(LOG_DBG_MT_MSG_traffic, pMsg, "*** Forwarding %s to (%s -> %s)."
        "Sequence ID: %d Length: %d\n", cp,
        pMsg->pSrcIface->dbg_name, pMsg->pDestIface->dbg_name,
        pMsg->sequence_id, pMsg->expected_len);
}

/*
 * We have received an SREQ from the Socket (connection)
 * We need to forward the request to the uart interface
 * and wait for the SRSP response.
 */
static void forward_sreq(struct mt_msg *pMsg)
{
    int r;
    struct mt_msg *pSrsp;

    say_forward(pMsg);

    r = uart_txrx(pMsg);
    if (r != 2)
    {
        MT_MSG_log(LOG_ERROR, pMsg, "Error relaying this message, r=%d\n", r);
        return;
    }

    pSrsp = pMsg->pSrsp;
    if (pSrsp == NULL)
    {
        return;
    }

    /* set destination */
    MT_MSG_setDestIface(pSrsp, pMsg->pSrcIface);

    /* We must reformat data to send to the socket */
    MT_MSG_reformat(pSrsp);

    say_forward(pSrsp);

    MT_MSG_txrx(pSrsp);
}

/*
 * We have received an AREQ from the socket we just forward it
 */
static void forward_areq(struct mt_msg *pMsg)
{
    say_forward(pMsg);
    MT_MSG_txrx(pMsg);
}

/*
 * We have received a POLL request from the socket, we just forward it.
 */
static void forward_poll(struct mt_msg *pMsg)
{
    say_forward(pMsg);
    MT_MSG_txrx(pMsg);
}

/*
 * This is the "socket to uart" thread
 * there is one "socket to uart" thread for each active connection.
 *
 * It is responsible for listening to the socket and forwarding
 * requests to the uart interface.
 */
static intptr_t s2u_thread(intptr_t cookie)
{
    struct npi_connection *pCONN;
    struct mt_msg *pMsg;
    int r;
    char iface_name[30];
    char star_line[30];
    int star_line_char;

    pCONN = (struct npi_connection *)(cookie);

    /* create our upstream interface {the accepted socket} */
    (void)snprintf(iface_name, sizeof(iface_name),
                   "s2u-%d-iface",
                   pCONN->connection_id);
    pCONN->socket_interface.dbg_name = iface_name;
    r = MT_MSG_interfaceCreate(&(pCONN->socket_interface));
    if (r != 0)
    {
        BUG_HERE("Cannot create socket interface?\n");
    }

    /* we are ready .. so mark */
    pCONN->socket_ready = true;

    /* wait for uart to sync */
    /* this should be fast ... so we just poll */
    while (!(pCONN->uart_ready))
    {
        TIMER_sleep(10);
    }

    star_line_char = 0;
    /* Wait for messages to come in from the socket.. */
    pCONN->s2u_busy = true;
    for (;;)
    {
        /* Did the other guy die? */
        if (pCONN->is_dead)
        {
            break;
        }

        /* did the socket die? */
        if (pCONN->socket_interface.is_dead)
        {
            pCONN->is_dead = true;
            continue;
        }

        if (STREAM_isError(common_uart_interface.hndl))
        {
            pCONN->is_dead = true;
            LOG_printf(LOG_ERROR, "%s: UART dead - closing socket\n",
                       THREAD_selfName());
            continue;
        }

        /* get our message */
        pMsg = MT_MSG_LIST_remove(&(pCONN->socket_interface),
                                   &(pCONN->socket_interface.rx_list),
                                   1000);
        if (pMsg == NULL)
        {
            /* must have timed out. */
            continue;
        }

        /* basically send it directly to the uart */
        MT_MSG_setDestIface(pMsg, &(common_uart_interface));
        /* reformat to send to the uart */
        MT_MSG_reformat(pMsg);

        star_line_char++;
        star_line_char = star_line_char % 26;
        memset((void *)(star_line), star_line_char + 'A',
               sizeof(star_line) - 1);
        star_line[sizeof(star_line) - 1] = 0;
        LOG_printf(LOG_DBG_MT_MSG_traffic, "START MSG: %s\n", star_line);

        switch (pMsg->m_type)
        {
        default:
            MT_MSG_log(LOG_ERROR,
                       pMsg,
                       "Unknown msg type: 0x%02x\n",
                       pMsg->m_type);
            break;
        case MT_MSG_TYPE_sreq:
            forward_sreq(pMsg);
            break;
        case MT_MSG_TYPE_areq:
            forward_areq(pMsg);
            break;
        case MT_MSG_TYPE_poll:
            forward_poll(pMsg);
            break;
        }
        LOG_printf(LOG_DBG_MT_MSG_traffic, "END MSG: %s\n", star_line);
        MT_MSG_free(pMsg);
        pMsg = NULL;
    }
    pCONN->s2u_busy = false;

    while( pCONN->u2s_busy ){
        // wake up the other side
        LOG_printf(LOG_DBG_MT_MSG_traffic, "Wait for u2s to finish\n");
        TIMER_sleep( 250 );
    }

    /* socket is dead */
    /* we need to destroy the interface */
    MT_MSG_interfaceDestroy(&(pCONN->socket_interface));

    /* destroy the socket. */
    SOCKET_ACCEPT_destroy(pCONN->socket_interface.hndl);

    /* we do not destroy free the connection */
    /* this is done in the uart thread */
    return 0;
}

static intptr_t uart_thread(intptr_t _notused)
{
    int r;
    struct mt_msg *pMsg;
    struct mt_msg *pSend;
    struct npi_connection *pCONN;

    (void)(_notused);
    pCONN = NULL;
    pMsg  = NULL;
    pSend = NULL;

    uart_mutex = MUTEX_create("uart-mutex");
    if(uart_mutex == 0)
    {
        BUG_HERE("Cannot create uart mutex\n");
    }

    r = MT_MSG_interfaceCreate(&(common_uart_interface));

    if(r != 0)
    {
        BUG_HERE("Cannot create uart interface\n");
    }

    LOG_printf(LOG_ALWAYS, "UART connection established on port: %s\n",
        common_uart_interface.u_cfg->devname);

#ifndef IS_HEADLESS
    fprintf(stdout, "UART connection established on port: %s\n",
        common_uart_interface.u_cfg->devname);
#endif //IS_HEADLESS

    uart_thread_ready = true;
    /* wait for the server to come up */

    while(!server_thread_ready)
    {
        TIMER_sleep(10);
    }

    /* process incoming (non-sreq, non-fragment) messages on the uart. */
    for(;;)
    {
        /* did this thing die? */
        if(STREAM_isError(common_uart_interface.hndl))
        {
            /* the uart has died... we exit now. */
            break;
        }

        /* wait for an areq to come from the uart.. */
        pMsg = MT_MSG_LIST_remove(
            &common_uart_interface,
            &(common_uart_interface.rx_list), 1000);
        if(pMsg == NULL)
        {
            continue;
        }

        /* and send it to all active connections. */
        lock_connection_list();
        pCONN = all_connections;
        while(pCONN)
        {
            /* assume we can send *THIS* message */
            pSend = pMsg;
            /* if there are more connections.. */
            if(pCONN->pNext)
            {
                /* then we send a clone instead */
                pSend = MT_MSG_clone(pSend);
                if(pSend == NULL)
                {
                    /* nothing here clone printed an error */
                }
            }

            /* handle case where clone failed, don't send null pointer */
            if(pSend)
            {
                /* and give it to the connections thread to process */
                MT_MSG_LIST_insert(&(pCONN->socket_interface),
                                    &(pCONN->areq_list),
                                    pSend);
                /* done.. */
                pSend = NULL;
            }
            /* next thread pair. */
            pCONN = pCONN->pNext;
        }
        unlock_connection_list();
    }

    uart_thread_ready = 0;
    /* Destroy the uart */
    MT_MSG_interfaceDestroy(&common_uart_interface);
    return 0;
}

static intptr_t server_thread(intptr_t _notused)
{
    (void)(_notused);

    int r;
    intptr_t server_handle;
    struct npi_connection *pCONN;
    int connection_id;
    char buf[30];

    pCONN = NULL;
    connection_id = 0;

    all_connections_mutex = MUTEX_create("all-connections");

    if(all_connections_mutex == 0)
    {
        BUG_HERE("Cannot create connection list mutex\n");
    }

    server_handle = SOCKET_SERVER_create(&my_socket_cfg);
    if(server_handle == 0)
    {
        BUG_HERE("Cannot create server socket\n");
    }

    r = SOCKET_SERVER_listen(server_handle);
    if(r != 0)
    {
        BUG_HERE("Cannot set server socket to listen mode\n");
    }

    LOG_printf(LOG_ALWAYS, "Socket server listening on port: %s\n",
        my_socket_cfg.service);

#ifndef IS_HEADLESS
    fprintf(stdout, "Socket server listening on port: %s\n",
        my_socket_cfg.service);
#endif //IS_HEADLESS

    /* we are ready.. */
    server_thread_ready = true;

    while(!uart_thread_ready)
    {
        TIMER_sleep(10);
    }

    /* Wait for connections :-) */
    for(;;)
    {
        if(STREAM_isError(server_handle))
        {
            LOG_printf(LOG_ERROR, "Server (accept) socket is dead\n");
            break;
        }
        if(pCONN == NULL)
        {
            pCONN = calloc(1, sizeof(*pCONN));
            if(pCONN == NULL)
            {
                BUG_HERE("No memory\n");
            }
            pCONN->connection_id = connection_id++;

            /* clone the connection details */
            pCONN->socket_interface = socket_interface_template;

            (void)snprintf(buf,sizeof(buf), "connection-%d", pCONN->connection_id);
            pCONN->dbg_name = strdup(buf);
            if(pCONN->dbg_name == NULL)
            {
                BUG_HERE("No memory\n");
            }
            MT_MSG_LIST_create(&(pCONN->areq_list), pCONN->dbg_name, "areq");
        }

        /* wait for a connection.. */
        r = SOCKET_SERVER_accept(&(pCONN->socket_interface.hndl), server_handle, 5000);
        if(r < 0)
        {
            BUG_HERE("Cannot accept!\n");
        }
        if(r == 0)
        {
            LOG_printf(LOG_ALWAYS, "No new server connections\n");
            continue;
        }
        /* we have a connection */
        pCONN->is_dead = false;

        /* add to the list of connections. */
        lock_connection_list();
        pCONN->pNext = all_connections;
        all_connections = pCONN;
        unlock_connection_list();

        /* create our connection threads */

        /* set name final use */
        (void)snprintf(buf, sizeof(buf),
                  "connection-%d",
                  pCONN->connection_id);
        pCONN->dbg_name = strdup(buf);
        if(pCONN->dbg_name == NULL)
        {
            BUG_HERE("No memory\n");
        }

        LOG_printf(LOG_ALWAYS, "Socket connection established. Port %s Id: %d\n",
            my_socket_cfg.service, pCONN->connection_id);

#ifndef IS_HEADLESS
        fprintf(stdout, "Socket connection established. Port %s Id: %d\n",
            my_socket_cfg.service, pCONN->connection_id);
#endif //IS_HEADLESS

        (void)snprintf(buf, sizeof(buf),
                  "u2s-%d",
                  pCONN->connection_id);
        pCONN->thread_id_u2s = THREAD_create(buf,
                                              u2s_thread,
                                              (intptr_t)(pCONN),
                                              THREAD_FLAGS_DEFAULT);
        if( pCONN->thread_id_u2s == 0 )
        {
            BUG_HERE("Cannot create uart to socket thread for connection: %d\n",
                     pCONN->connection_id );
        }

        (void)snprintf(buf, sizeof(buf),
                  "s2u-%d",
                  pCONN->connection_id);
        pCONN->thread_id_s2u = THREAD_create(buf,
                                              s2u_thread,
                                              (intptr_t)(pCONN),
                                              THREAD_FLAGS_DEFAULT);

        if( pCONN->thread_id_u2s == 0 )
        {
            BUG_HERE("Cannot create socket to uart thread for connection: %d\n",
                     pCONN->connection_id );
        }
        /* We passed the connection pointer to the thread */
        /* we no longer need this, so set the pointer to null */
        pCONN = NULL;

    }
    return 0;
}

void APP_main(void)
{
    int r;
    struct npi_connection *pCONN;

    uart_thread_id   = THREAD_create("uart-thread",
                                     uart_thread,
                                     0,
                                     THREAD_FLAGS_DEFAULT);
    server_thread_id = THREAD_create("server-thread",
                                     server_thread,
                                     0,
                                     THREAD_FLAGS_DEFAULT);

    for(;;)
    {
        /* every 30 seconds.. */
        /* no specific reason... for that time */
        TIMER_sleep(30 * 1000);
        r = 0;
        if(THREAD_isAlive(uart_thread_id))
        {
            r += 1;
        }
        if(THREAD_isAlive(server_thread_id))
        {
            r += 1;
        }
        /* Keep looping as long either
           the downstream (uart) side is alive
           or the upstream server side is alive.
         */
        if(r == 0)
        {
            /* once both are dead... break */
            break;
        }
    }

    /* wait at most 10 seconds then we just 'die' */
    r = 0;
    while(r < 10)
    {

        lock_connection_list();
        pCONN = all_connections;
        /* mark them all as dead */
        while(pCONN)
        {
            pCONN->is_dead = true;
            pCONN = pCONN->pNext;
        }
        unlock_connection_list();

        /* still have connections? */
        if(all_connections)
        {
            /* wait a second.. */
            TIMER_sleep(1000);
            /* and try again */
            r++;
            continue;
        }
        else
        {
            break;
        }
    }
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
