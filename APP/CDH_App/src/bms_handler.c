#include "bms_handler.h"

#include <asn1_lfp.h>
#include <lfp_i2c_single.h>
#include <threads.h>
#include <asn1/bms.h>
#include <asn1/_systems.h>
#include <lfp/stream.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(bms_handler);

#define BMS_LOGGING_INTERVAL 10
#define BMS_HEATER_INTERVAL 10

#define BMS_I2C_READ_TIMEOUT 10

static const struct i2c_dt_spec dev_bms = I2C_DT_SPEC_GET(DT_NODELABEL(bms_i2c));
K_MUTEX_DEFINE(dev_bms_lock);

static inline double bms_adc_to_float(const uint16_t val) {
    return ((double)val * 3.3) / (1 << 12);
}

static inline uint16_t bms_float_to_adc(const double val) {
    return (uint16_t)((val * (double)(1 << 12)) / 3.3);
}

static uint8_t lfp_send_buf[MAX_FROM_LIST( // NOLINT(bugprone-branch-clone)
    ASN1_LFP_SEND_BUF_SIZE(BMSPowerStatusRequest),
    ASN1_LFP_SEND_BUF_SIZE(BMSTemperatureStatusRequest),
    ASN1_LFP_SEND_BUF_SIZE(BMSSystemStatusRequest),
    ASN1_LFP_SEND_BUF_SIZE(BMSSetHeaterDutyRequest)
)];
static uint8_t lfp_recv_buf[MAX_FROM_LIST( // NOLINT(bugprone-branch-clone)
    ASN1_LFP_RECV_BUF_SIZE(BMSPowerStatusResponse),
    ASN1_LFP_RECV_BUF_SIZE(BMSTemperatureStatusResponse),
    ASN1_LFP_RECV_BUF_SIZE(BMSSystemStatusResponse),
    ASN1_LFP_RECV_BUF_SIZE(BMSSetHeaterDutyResponse)
)];

static void bms_logging_log_battery(const BMSBatteryPackStatus * p_pack, const int pack_id) {
    const BMSCellStatus * p_cell_a = &p_pack->cellA;
    const BMSCellStatus * p_cell_b = &p_pack->cellB;

    LOG_INF(
        "Battery %d:\n\tVoltage: %.3f | Current Draw: %.3f | Current Charge: %.3f | Overcurrent: %d\n"
        "\tCell A:\n\t\tVoltage: %.3f\n\t\tOV/UV: %d/%d\n"
        "\tCell B:\n\t\tVoltage: %.3f\n\t\tOV/UV: %d/%d",
        pack_id,
        bms_adc_to_float(p_pack->voltage), bms_adc_to_float(p_pack->currentDraw),
        bms_adc_to_float(p_pack->currentCharge),
        p_pack->overcurrent,

        bms_adc_to_float(p_cell_a->voltage),
        p_cell_a->overvoltage, p_cell_a->undervoltage,

        bms_adc_to_float(p_cell_b->voltage),
        p_cell_b->overvoltage, p_cell_b->undervoltage
    );

}

static void bms_logging_callback(BMSPowerStatusResponse * p_payload, asn1_lfp_decode_data_t * p_data) {
    ARG_UNUSED(p_data);

    bms_logging_log_battery(&p_payload->batteryPack1, 1);
    bms_logging_log_battery(&p_payload->batteryPack2, 2);
}


static void test_function(int error_code, const struct asn1_lfp_decode_data_t* p_data) {
    ARG_UNUSED(p_data);
    LOG_WRN("ASN1 parse failure: %d", error_code);
}

static void bms_logging_thread(const void *p1, const void *p2, const void *p3) {
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (true) {
        k_mutex_lock(&dev_bms_lock, K_FOREVER);

        {
            const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, bmsSystemId, BMSPowerStatusRequest, lfp_send_buf, sizeof(lfp_send_buf), {});
            if (!size) {
                LOG_WRN("Failed to serialize power status request: a:%d l:%d",
                    ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
                goto unlock;
            }

            LOG_HEXDUMP_INF(lfp_send_buf, size, "Sending:");

            const int ret = i2c_write_dt(&dev_bms, lfp_send_buf, size);
            if (ret != 0) {
                LOG_WRN("Failed to send power status request: %d", ret);
                goto unlock;
            }
        }

        //TODO: remove
        {
            int ret = i2c_write_dt(&dev_bms, (uint8_t[]){0xFF}, 0 );
            if (ret != 0) {
                LOG_INF("Failed to send zero request: %d", ret);
            } else {
                LOG_WRN("Zero request was successful: %d", ret);
            }
            ret = i2c_write_dt(&dev_bms, (uint8_t[]){0xFF}, 1);
            if (ret != 0) {
                LOG_INF("Failed to send extra request: %d", ret);
            } else {
                LOG_WRN("Extra request was successful: %d", ret);
            }

            uint8_t buf[1];
            int ret2 = i2c_read_dt(&dev_bms, buf, 1);
            if (ret2 != 0) {
                LOG_WRN("Failed to send extra read: %d", ret2);
            } else {
                LOG_INF("Extra read: %d", buf[0]);
            }

            ret2 = i2c_read_dt(&dev_bms, buf, 1);
            if (ret2 != 0) {
                LOG_WRN("Failed to send extra read 2: %d", ret2);
            } else {
                LOG_INF("Extra read2: %d", buf[0]);
            }
        }

        LOG_INF("Request sent!");

        k_sleep(K_SECONDS(1));

        lfp_header_t header;
        asn1_lfp_decode_data_t data = {.p_on_error_cb = test_function, .p_header = &header, .p_body = lfp_recv_buf};
        data.body_length = lfp_i2c_single_receive(
            &dev_bms,
            &header,
            lfp_recv_buf, sizeof(lfp_recv_buf),
            K_MSEC(100));
        if (data.body_length == LFP_FAIL) {
            LOG_WRN("Failed to read BMS response");
            goto unlock;
        }

        if (ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSPowerStatusResponse, bms_logging_callback)) {
            goto unlock;
        }

        LOG_WRN("Bad message handler!");

unlock:
        k_mutex_unlock(&dev_bms_lock);

        k_sleep(K_SECONDS(BMS_LOGGING_INTERVAL));
    }
}

static void bms_heater_thread(const void *p1, const void *p2, const void *p3) {
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (true) {
        k_mutex_lock(&dev_bms_lock, K_FOREVER);


        // ASN1_LFP_SERIALIZE(cdhSystemId, bmsSystemId, BMSTemperatureStatusRequest, lfp_i2c_send_chr, (void*)&dev_bms, {0});



        k_mutex_unlock(&dev_bms_lock);

        k_sleep(K_SECONDS(BMS_HEATER_INTERVAL));
    }
}

// K_THREAD_DEFINE(p_bms_heater_thread, 1024, bms_heater_thread, NULL, NULL, NULL,
//         THREAD_PRIORITY_BMS_HEATER, 0, 0);
// Stagger the threads so that they don't fight over the device
K_THREAD_DEFINE(p_bms_logging_thread, 2024, bms_logging_thread, NULL, NULL, NULL,
        THREAD_PRIORITY_BMS_LOGGING, 0, 0);
