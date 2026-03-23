#include "server.h"

#include <csp/csp.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>


#include "main.h"

LOG_MODULE_REGISTER(server);


static void server_task(void * p1, void * p2, void * p3) {
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

	LOG_INF("Server task started");

	/* Create socket with no specific socket options, e.g. accepts CRC32, HMAC, etc. if enabled during compilation */
	csp_socket_t sock = {0};

	/* Bind socket to all ports, e.g. all incoming connections will be handled here */
	csp_bind(&sock, CSP_ANY);

	/* Create a backlog of 10 connections, i.e. up to 10 new connections can be queued */
	csp_listen(&sock, 10);

	/* Wait for connections and then process packets on the connection */
	while (1) {

		/* Wait for a new connection, 10000 mS timeout */
		csp_conn_t *conn;
		if ((conn = csp_accept(&sock, 10000)) == NULL) {
			/* timeout */
			continue;
		}

		/* Read packets on connection, timeout is 100 mS */
		csp_packet_t *packet;
		while ((packet = csp_read(conn, 50)) != NULL) {
			switch (csp_conn_dport(conn)) {
			case SERVER_PORT:
				/* Process packet here */
				LOG_HEXDUMP_INF(packet->data, packet->length, "Packet received on MY_SERVER_PORT:");
				csp_buffer_free(packet);
				break;

			default:
				/* Call the default CSP service handler, handle pings, buffer use, etc. */
				csp_service_handler(packet);
				break;
			}
		}

		/* Close current connection */
		csp_close(conn);

	}
}

K_THREAD_DEFINE(p_server_thread, 1024,
                server_task, NULL, NULL, NULL,
                0, 0, K_TICKS_FOREVER);

void start_server(void)
{
	k_thread_start(p_server_thread);
}