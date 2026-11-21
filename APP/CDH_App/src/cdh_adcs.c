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


static void on_error(int error_code, const struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG("ADCS ASN.1 parse failure: %d", error_code);
}

static void adcs_system_status_callback(
    ADCSSystemStatusResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG(
      "ADCS System Status: version=%u uptime=%u adcs=%u magnetometer=%u "
      "sunSensor=%u gyro=%u magnetorquer=%u",
      p_payload->version,
      p_payload->uptime,
      p_payload->adcsState,
      p_payload->magnetometerState,
      p_payload->sunSensorState,
      p_payload->gyroState,
      p_payload->magnetorquerState);
}

static void adcs_position_callback(
    ADCSPositionResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG(
      "ADCS Position: timestamp=%u.%03u valid=%u latitude=%d "
      "longitude=%d altitude=%d",
      p_payload->timestamp.seconds,
      p_payload->timestamp.milliseconds,
      p_payload->valid,
      p_payload->latitude,
      p_payload->longitude,
      p_payload->altitude);
}

static void adcs_velocity_callback(
    ADCSVelocityResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG(
      "ADCS Velocity: timestamp=%u.%03u valid=%u vx=%d vy=%d vz=%d",
      p_payload->timestamp.seconds,
      p_payload->timestamp.milliseconds,
      p_payload->valid,
      p_payload->vx,
      p_payload->vy,
      p_payload->vz);
}

static void adcs_attitude_callback(
    ADCSAttitudeResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG(
      "ADCS Attitude: timestamp=%u.%03u valid=%u roll=%d pitch=%d yaw=%d",
      p_payload->timestamp.seconds,
      p_payload->timestamp.milliseconds,
      p_payload->valid,
      p_payload->roll,
      p_payload->pitch,
      p_payload->yaw);
}

static void adcs_attitude_accuracy_callback(
    ADCSAttitudeAccuracyResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG(
      "ADCS Attitude Accuracy: timestamp=%u.%03u accuracy=%u",
      p_payload->timestamp.seconds,
      p_payload->timestamp.milliseconds,
      p_payload->accuracy);
}

static void adcs_get_mode_callback(
    ADCSGetModeResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG(
      "ADCS Mode: currentMode=%u timeInMode=%u",
      p_payload->currentMode,
      p_payload->timeInMode);
}

static void adcs_set_mode_callback(
    ADCSSetModeResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG(
      "ADCS Set Mode: result=%u currentMode=%u",
      p_payload->result,
      p_payload->currentMode);
}

static void adcs_power_estimate_callback(
    ADCSPowerEstimateResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG(
      "ADCS Power Estimate: timestamp=%u.%03u estimatedPower=%u",
      p_payload->timestamp.seconds,
      p_payload->timestamp.milliseconds,
      p_payload->estimatedPower);
}

static void adcs_magnetic_field_callback(
    ADCSMagneticFieldResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG(
      "ADCS Magnetic Field: timestamp=%u.%03u valid=%u bx=%d by=%d bz=%d",
      p_payload->timestamp.seconds,
      p_payload->timestamp.milliseconds,
      p_payload->valid,
      p_payload->bx,
      p_payload->by,
      p_payload->bz);
}

static void adcs_sun_measurement_callback(
    ADCSSunMeasurementResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG(
      "ADCS Sun Measurement: timestamp=%u.%03u visible=%u "
      "azimuth=%d elevation=%d",
      p_payload->timestamp.seconds,
      p_payload->timestamp.milliseconds,
      p_payload->sunVisible,
      p_payload->azimuth,
      p_payload->elevation);
}

static void adcs_temperature_callback(
    ADCSTemperatureResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG(
      "ADCS Temperatures: %d, %d, %d, %d",
      p_payload->temperatures.arr[0],
      p_payload->temperatures.arr[1],
      p_payload->temperatures.arr[2],
      p_payload->temperatures.arr[3]);
}

static void adcs_error_callback(
    ADCSErrorResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG(
      "ADCS Errors: lastErrorCode=%u lastErrorTime=%u.%03u errorCount=%u",
      p_payload->lastErrorCode,
      p_payload->lastErrorTime.seconds,
      p_payload->lastErrorTime.milliseconds,
      p_payload->errorCount);
}

static void adcs_torque_command_callback(
    ADCSTorqueCommandResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG(
      "ADCS Torque: timestamp=%u.%03u torque=(%d,%d,%d) "
      "mtqDuty=(%d,%d,%d)",
      p_payload->timestamp.seconds,
      p_payload->timestamp.milliseconds,
      p_payload->torqueX,
      p_payload->torqueY,
      p_payload->torqueZ,
      p_payload->mtqDuty.arr[0],
      p_payload->mtqDuty.arr[1],
      p_payload->mtqDuty.arr[2]);
}

static void adcs_set_tle_callback(
    ADCSSetTLEResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  LOG_DBG("ADCS Set TLE: %s", p_payload->result.arr);
}

static void adcs_housekeeping_callback(
    ADCSHousekeepingResponse *p_payload,
    struct asn1_lfp_decode_data_t *p_data)
{
  ARG_UNUSED(p_data);
  adcs_system_status_callback(&p_payload->systemStatus, p_data);
  adcs_position_callback(&p_payload->position, p_data);
  adcs_velocity_callback(&p_payload->velocity, p_data);
  adcs_attitude_callback(&p_payload->attitude, p_data);
  adcs_attitude_accuracy_callback(&p_payload->attitudeAccuracy, p_data);
  adcs_get_mode_callback(&p_payload->mode, p_data);
  adcs_power_estimate_callback(&p_payload->powerEstimate, p_data);
  adcs_magnetic_field_callback(&p_payload->magneticField, p_data);
  adcs_sun_measurement_callback(&p_payload->sunMeasurement, p_data);
  adcs_temperature_callback(&p_payload->temperature, p_data);
  adcs_error_callback(&p_payload->error, p_data);
  adcs_torque_command_callback(&p_payload->torqueCommand, p_data);
}
