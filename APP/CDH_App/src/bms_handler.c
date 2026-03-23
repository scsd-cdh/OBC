#include "bms_handler.h"

#include <asn1_lfp.h>
#include <lfp_i2c_single.h>
#include <threads.h>
#include <asn1/bms.h>
#include <asn1/_systems.h>
#include <lfp/stream.h>
#include <ulog/ulog.h>
#include <zephyr/drivers/i2c.h>

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

    ULOG_INFO(
        "Battery {}: \n\tVoltage: {:.3} | Current Draw: {:.3} | Current Charge: {:.3}\n\tCell A:\n\t\tVoltage: {:.3}\n\t\tOV/UV/OC/UC: {}/{}/{}/{}\n\tCell B:\n\t\tVoltage: {:.3}\n\t\tOV/UV/OC/UC: {}/{}/{}/{}",
        pack_id,
        bms_adc_to_float(p_pack->voltage),bms_adc_to_float(p_pack->currentDraw), bms_adc_to_float(p_pack->currentCharge),

        bms_adc_to_float(p_cell_a->voltage),
        p_cell_a->overvoltage, p_cell_a->undervoltage, p_cell_a->overcurrent, p_cell_a->undervoltage,

        bms_adc_to_float(p_cell_b->voltage),
        p_cell_b->overvoltage, p_cell_b->undervoltage, p_cell_b->overcurrent, p_cell_b->undervoltage,
    );

}

static void bms_logging_callback(BMSPowerStatusResponse * p_payload, asn1_lfp_decode_data_t * p_data) {
    ARG_UNUSED(p_data);

    bms_logging_log_battery(&p_payload->batteryPack1, 1);
    bms_logging_log_battery(&p_payload->batteryPack2, 2);
}


static void test_function(int error_code, const struct asn1_lfp_decode_data_t* p_data) {
    ULOG_WARN("ASN1 parse failure: {}", error_code);
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
                ULOG_WARN("Failed to serialize power status request: a:{} l:{}", ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
                goto unlock;
            }

            ULOG_INFO("Sending {}", ((struct ulog_slice) {.data = lfp_send_buf, .size=size}));

            const int ret = i2c_write_dt(&dev_bms, lfp_send_buf, size);
            if (ret != 0) {
                ULOG_WARN("Failed to send power status request: {}", ret);
                goto unlock;
            }
        }

        //TODO: remove
        {
            int ret = i2c_write_dt(&dev_bms, (uint8_t[]){0xFF}, 0 );
            if (ret != 0) {
                ULOG_INFO("Failed to send zero request: {}", ret);
            } else {
                ULOG_WARN("Zero request was successful: {}", ret);
            }
            ret = i2c_write_dt(&dev_bms, (uint8_t[]){0xFF}, 1);
            if (ret != 0) {
                ULOG_INFO("Failed to send extra request: {}", ret);
            } else {
                ULOG_WARN("Extra request was successful: {}", ret);
            }

            uint8_t buf[1];
            int ret2 = i2c_read_dt(&dev_bms, buf, 1);
            if (ret2 != 0) {
                ULOG_WARN("Failed to send extra read: {}", ret2);
            } else {
                ULOG_INFO("Extra read: {}", buf[0]);
            }

            ret2 = i2c_read_dt(&dev_bms, buf, 1);
            if (ret2 != 0) {
                ULOG_WARN("Failed to send extra read 2: {}", ret2);
            } else {
                ULOG_INFO("Extra read2: {}", buf[0]);
            }
        }

        ULOG_INFO("Request sent!");

        k_sleep(K_SECONDS(1));

        lfp_header_t header;
        asn1_lfp_decode_data_t data = {.p_on_error_cb = test_function, .p_header = &header, .p_body = lfp_recv_buf};
        data.body_length = lfp_i2c_single_receive(
            &dev_bms,
            &header,
            lfp_recv_buf, sizeof(lfp_recv_buf),
            K_MSEC(100));
        if (data.body_length == LFP_FAIL) {
            ULOG_WARN("Failed to read BMS response");
            goto unlock;
        }

        if (ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSPowerStatusResponse, bms_logging_callback)) {
            goto unlock;
        }

        ULOG_WARN("Bad message handler!");

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
