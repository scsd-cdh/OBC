//static const struct device * dev_demo_uart = DEVICE_DT_GET(DT_NODELABEL(demo_serial));
void asn1_example_cb(uint8_t chr, void * p_ctx) {
    //uart_poll_out(dev_demo_uart, chr);
}

void asn1_example(void) {
    ASN1_LFP_SERIALIZE(cdhSystemId, bmsSystemId, BMSSystemStatusResponse, asn1_example_cb, NULL, {
        .uptime = 0xDEADBEEF,
        .version = 12
    });

    if (!ASN1_LFP_SUCCESS()) {
        ULOG_ERROR("LFP encoding failed! {} {}", ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
    }
}

void asn1_example_bms_system_status_req(const BMSSystemStatusRequest * p_payload, const asn1_lfp_decode_data_t * p_data) {
    ULOG_INFO("Received system status request");
}
void asn1_example_bms_system_status_res(const BMSSystemStatusResponse * p_payload, const asn1_lfp_decode_data_t * p_data) {
    ULOG_INFO("Received system status response: version {}, uptime {}", p_payload->version, p_payload->uptime);
}

void asn1_example_error_handler(const int asn1_error, const asn1_lfp_decode_data_t * p_data) {
    ULOG_ERROR("Failed to decode message {} (error {})", lfp_composite_id(p_data->p_header), asn1_error);
}

void asn1_example_on_msg(const lfp_header_t * p_header, const uint8_t * p_body, const uint16_t body_length, void * p_ctx) {
    ARG_UNUSED(p_ctx);
    ARG_UNUSED(body_length);

    ULOG_INFO("Decoded packet on endpoint {}: {}", p_header->endpoint, (char *)p_body);

    const asn1_lfp_decode_data_t data = {
        .p_header = p_header,
        .p_body = p_body,
        .p_ctx = p_ctx,
        .body_length = body_length,
        .p_on_error_cb = asn1_example_error_handler
    };

    ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSSystemStatusRequest, asn1_example_bms_system_status_req)
    else ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSSystemStatusResponse, asn1_example_bms_system_status_res)
    else {
        ULOG_WARN("Received unknown message {}", ULOG_SLICE_PTR((lfp_header_t *)p_header));
    }
}