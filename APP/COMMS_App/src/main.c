#include "main.h"

#include <stdlib.h>
#include <csp/csp.h>
#include <csp/drivers/usart.h>
#include <csp/interfaces/csp_if_lo.h>
#include <ulog/ulog.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "server.h"
#include "test_client.h"

static void router_task(void * p1, void * p2, void * p3) {
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

    /* Here there be routing */
    while (1) {
        csp_route_work();
    }
}

K_THREAD_DEFINE(p_router_thread, 256,
                router_task, NULL, NULL, NULL,
                0, 0, K_TICKS_FOREVER);
typedef struct {
	char name[CSP_IFLIST_NAME_MAX + 1];
	csp_usart_callback_t rx_callback;
	void * user_data;
	csp_usart_fd_t fd;
	struct k_thread rx_thread;
	int cbuf_len;
	uint8_t cbuf[CONFIG_CSP_UART_RX_BUFFER_LENGTH + 1];
} usart_context_t;

int main(void) {
	printk("Size: %d", sizeof(usart_context_t));
	const char * kiss_device = DEVICE_DT_NAME(DT_ALIAS(transmitter));

	ULOG_INFO("Initialising CSP");

	csp_conf.version = 1;

	/* Init CSP */
	csp_init();

	/* Start router */
	k_thread_start(p_router_thread);

	/* Add interface(s) */
	csp_iface_t * p_iface = NULL;
	const csp_usart_conf_t conf = {
		.device = kiss_device,
		.baudrate = 500000,
		.databits = 8,
		.stopbits = 1,
		.paritysetting = 0,
	};
	const int error = csp_usart_open_and_add_kiss_interface(&conf, CSP_IF_KISS_DEFAULT_NAME, SERVER_ADDRESS, &p_iface);
	if (error != CSP_ERR_NONE) {
		ULOG_ERROR("failed to add KISS interface [{}], error: {}", kiss_device, error);
		exit(1);
	}
	p_iface->is_default = 1;
	// csp_if_lo.is_default = 1;

	/* Start server thread */
	start_server();

	/* Start client thread */
	// start_client();
}