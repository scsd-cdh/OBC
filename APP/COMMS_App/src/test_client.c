#include "test_client.h"

#include <csp/csp.h>
#include <ulog/ulog.h>

#include "main.h"

static void client_task(void * p1, void * p2, void * p3) {
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    ULOG_INFO("Client task started");

    unsigned int count = 'A';

    while (1) {

        k_sleep(K_USEC(200000));

        /* Send ping to server, timeout 1000 mS, ping size 100 bytes */
        int __maybe_unused result = csp_ping(SERVER_ADDRESS, 1000, 100, CSP_O_NONE);
        ULOG_INFO("Ping address: {}, result {} [mS]", SERVER_ADDRESS, result);

        /* Send data packet (string) to server */

        /* 1. Connect to host on 'server_address', port MY_SERVER_PORT with regular UDP-like protocol and 1000 ms timeout */
        csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, SERVER_ADDRESS, SERVER_PORT, 1000, CSP_O_NONE);
        if (conn == NULL) {
            /* Connect failed */
            ULOG_ERROR("Connection failed");
            return;
        }

        /* 2. Get packet buffer for message/data */
        csp_packet_t * packet = csp_buffer_get(0);
        if (packet == NULL) {
            /* Could not get buffer element */
            ULOG_ERROR("Failed to get CSP buffer");
            return;
        }

        /* 3. Copy data to packet */
        memcpy(packet->data, "Hello world ", 12);
        memcpy(packet->data + 12, &count, 1);
        memset(packet->data + 13, 0, 1);
        count++;

        /* 4. Set packet length */
        packet->length = (strlen((char *) packet->data) + 1); /* include the 0 termination */

        /* 5. Send packet */
        csp_send(conn, packet);

        /* 6. Close connection */
        csp_close(conn);
    }
}

K_THREAD_DEFINE(p_client_thread, 1024,
                client_task, NULL, NULL, NULL,
                0, 0, K_TICKS_FOREVER);

void start_client(void)
{
    k_thread_start(p_client_thread);
}