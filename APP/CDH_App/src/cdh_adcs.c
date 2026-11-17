#include <asn1/_systems.h>
#include <asn1/adcs.h>
#include <asn1_lfp.h>
#include <lfp/header.h>
#include <lfp_i2c_single.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(cdh_adcs);

#define I2C_TIMEOUT_MS K_MSEC(1000)


static uint8_t lfp_send_buf[MAX_FROM_LIST(
  MAX_FROM_LIST(
    ASN1_LFP_SEND_BUF_SIZE(ADCSSystemStatusRequest),
    ASN1_LFP_SEND_BUF_SIZE(ADCSPositionRequest),
    ASN1_LFP_SEND_BUF_SIZE(ADCSVelocityRequest),
    ASN1_LFP_SEND_BUF_SIZE(ADCSAttitudeRequest),
    ASN1_LFP_SEND_BUF_SIZE(ADCSAttitudeAccuracyRequest),
    ASN1_LFP_SEND_BUF_SIZE(ADCSGetModeRequest),
    ASN1_LFP_SEND_BUF_SIZE(ADCSSetModeRequest),
    ASN1_LFP_SEND_BUF_SIZE(ADCSPowerEstimateRequest),
    ASN1_LFP_SEND_BUF_SIZE(ADCSMagneticFieldRequest),
    ASN1_LFP_SEND_BUF_SIZE(ADCSSunMeasurementRequest)
  ),
  MAX_FROM_LIST(
    ASN1_LFP_SEND_BUF_SIZE(ADCSTemperatureRequest),
    ASN1_LFP_SEND_BUF_SIZE(ADCSErrorRequest),
    ASN1_LFP_SEND_BUF_SIZE(ADCSTorqueCommandRequest),
    ASN1_LFP_SEND_BUF_SIZE(ADCSSetTLERequest),
    ASN1_LFP_SEND_BUF_SIZE(ADCSHousekeepingRequest)
  ))];



static uint8_t lfp_recv_buf[MAX_FROM_LIST(
  MAX_FROM_LIST(
    ASN1_LFP_RECV_BUF_SIZE(ADCSSystemStatusResponse),
    ASN1_LFP_RECV_BUF_SIZE(ADCSPositionResponse),
    ASN1_LFP_RECV_BUF_SIZE(ADCSVelocityResponse),
    ASN1_LFP_RECV_BUF_SIZE(ADCSAttitudeResponse),
    ASN1_LFP_RECV_BUF_SIZE(ADCSAttitudeAccuracyResponse),
    ASN1_LFP_RECV_BUF_SIZE(ADCSGetModeResponse),
    ASN1_LFP_RECV_BUF_SIZE(ADCSSetModeResponse),
    ASN1_LFP_RECV_BUF_SIZE(ADCSPowerEstimateResponse),
    ASN1_LFP_RECV_BUF_SIZE(ADCSMagneticFieldResponse),
    ASN1_LFP_RECV_BUF_SIZE(ADCSSunMeasurementResponse)
  ),
  MAX_FROM_LIST(
    ASN1_LFP_RECV_BUF_SIZE(ADCSTemperatureResponse),
    ASN1_LFP_RECV_BUF_SIZE(ADCSErrorResponse),
    ASN1_LFP_RECV_BUF_SIZE(ADCSTorqueCommandResponse),
    ASN1_LFP_RECV_BUF_SIZE(ADCSSetTLEResponse),
    ASN1_LFP_RECV_BUF_SIZE(ADCSHousekeepingResponse)
  ))];


