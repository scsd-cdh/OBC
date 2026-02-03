#include "main.h"

#include <stdio.h>
#include <asn1scc/bms.h>
#include <ulog/ulog.h>
#include <zephyr/drivers/gpio.h>

int decoder(uint8_t *, int);

int main(void) {
    while (true) {
        // Create payload
        const PDU request = {
            .kind = request_PRESENT,
            .u = {
                .request = {
                    .kind = request_system_status_PRESENT,
                    .u = {
                        .system_status = {}
                    }
                }
            }
        };

        // Create buffer for encoded data
        unsigned char encodedBuffer[PDU_REQUIRED_BYTES_FOR_ENCODING];

        // Prepare encoder
        int errCode;
        BitStream encoder;
        BitStream_Init(&encoder, encodedBuffer, sizeof(encodedBuffer));

        // Do encoding
        if (!PDU_Encode(&request, &encoder, &errCode, true)) {
            ULOG_ERROR("Failed to encode PDU request: {}", errCode);
            return 1;
        }

        // Log bytes
        const int length = (int)BitStream_GetLength(&encoder);
        const struct ulog_slice slice = {
            .data = encodedBuffer,
            .size = length,
        };
        ULOG_INFO("Request encoded successfully: {}", slice);

        // Send the buffer to the decoder
        decoder(encodedBuffer, length);

        k_msleep(2000);
    }
}

int decoder(uint8_t * data, const int len) {
    // Setup decoder
    BitStream decoder;
    BitStream_Init(&decoder, data, len);
    int errCode;
    PDU request;

    // Attempt to decode
    if (!PDU_Decode(&request, &decoder, &errCode)) {
        ULOG_ERROR("Failed to decode PDU request: {}", errCode);
        return 1;
    }

    // Validate that its a request
    if (request.kind != request_PRESENT) {
        ULOG_ERROR("Request PDU was not a request");
        return 1;
    }

    // Validate that its a system_status request
    if (request.u.request.kind != request_system_status_PRESENT) {
        ULOG_ERROR("Unimplemented request PDU");
        return 1;
    }

    // Setup encoder for "reponse"
    BitStream encoder;
    unsigned char encodedBuffer[PDU_REQUIRED_BYTES_FOR_ENCODING];
    BitStream_Init(&encoder, encodedBuffer, sizeof(encodedBuffer));

    // Setup response payload
    const PDU response = {
        .kind = response_PRESENT,
        .u = {
            .response = {
                .kind = response_system_status_PRESENT,
                .u = {
                    .system_status = {
                        .uptime = 4242,
                        .version = 69
                    }
                }
            }
        }
    };

    // Encode response
    if (!PDU_Encode(&response, &encoder, &errCode, true)) {
        ULOG_ERROR("Failed to encode PDU response: {}", errCode);
        return 1;
    }

    // Log it
    const int length = (int)BitStream_GetLength(&encoder);
    const struct ulog_slice slice = {
        .data = encodedBuffer,
        .size = length,
    };
    ULOG_INFO("Response encoded successfully: {}", slice);

    return 0;
}
