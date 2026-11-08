#include "cdh_comms.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>

#include "lfp/stream.h"

LOG_MODULE_REGISTER(cdh_comms);

int cdh_comms_fwd_lfp_msg(
    const asn1_lfp_decode_data_t *data,
    uint8_t *send_buf,
    size_t send_buf_size
)
{
    return lfp_encode(
        data->p_header->initiator,
        data->p_header->target,
        data->p_header->endpoint,
        data->p_header->direction,
        data->p_body,
        data->body_length,
        send_buf,
        send_buf_size
    );
}



