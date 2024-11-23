#include "tinyprotocol.h"

// Configuration
int16_t CustomProcessTelecommand(uint8_t command, const uint8_t* buffer, uint8_t size);
int16_t CustomWriteBuffer(const uint8_t* buffer, uint8_t size);
int16_t CustomProcessTelemetryRequest();

static const struct TINYPROTOCOL_Config tinyprotocol_config = {
  CustomProcessTelecommand,
  CustomProcessTelemetryRequest,
  CustomWriteBuffer,
};

// Telecommands
enum CustomProtocolTelecommand {
  TC_TOGGLE_LED = TINYPROTOCOL_TC_RESERVED,
  TC_SET_LED
};

// Telemetry Requests
enum CustomProtocolTelemetryRequest {
  TLM_GET_LED_STATUS = TINYPROTOCOL_TLM_RESERVED,
};

static volatile bool led_status = false;

void setup() {
  // LED
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_BUILTIN, LOW);
  led_status = false;

  // Serial
  Serial.begin(115200);
  
  // Tinyprotocol initialization
  TINYPROTOCOL_Initialize();

  // Register telecommand
  int16_t err;
  err = TINYPROTOCOL_RegisterTelecommand(TC_TOGGLE_LED, 0);           // Empty telecommand, will always toggle the same LED
  if(err < 0) {
    pinMode(LED_BUILTIN, LOW);
    return;
  }
  
  err = TINYPROTOCOL_RegisterTelecommand(TC_SET_LED, 1);              // Telecommand with payload, will set builtin LED to high or low
  if(err < 0) {
    pinMode(LED_BUILTIN, LOW);
    return;
  }

  // Register telemetry channel
  err = TINYPROTOCOL_RegisterTelemetryChannel(TLM_GET_LED_STATUS, (const uint8_t*)&led_status, 1);
  if(err < 0) {
    pinMode(LED_BUILTIN, LOW);
    return;
  }

  pinMode(LED_BUILTIN, HIGH);
}

void loop() {
  while(Serial.available()) {
    uint8_t byte = Serial.read();
    TINYPROTOCOL_ParseByte(&tinyprotocol_config, byte);
  }
}

int16_t CustomProcessTelecommand(uint8_t command, const uint8_t* buffer, uint8_t size) {
  switch (command) {
    case TC_TOGGLE_LED:
      led_status = !led_status;
      digitalWrite(LED_BUILTIN, led_status);
      break;
    case TC_SET_LED:
      digitalWrite(LED_BUILTIN, buffer[0]);
      led_status = buffer[0];
      break;
  }

  return ETINYPROTOCOL_SUCCESS;
}

int16_t CustomProcessTelemetryRequest() {
  uint8_t byte;

  while(TINYPROTOCOL_TelemetryBytesLeft() > 0) {
    int16_t result = TINYPROTOCOL_ReadNextTelemetryByte(&byte);
    if (result == ETINYPROTOCOL_SUCCESS) {
      Serial.write(byte);
    } else {
      return result;
    }
  }

  return ETINYPROTOCOL_SUCCESS;
}

int16_t CustomWriteBuffer(const uint8_t* buffer, uint8_t size) {
  Serial.write(buffer, size);
  return ETINYPROTOCOL_SUCCESS;
}
