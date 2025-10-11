#include "LTR.h"

void ltr_init(ltr_t *ltr){
  Serial.println("Initializing LTR329...");
  
  // Test connection first
  if (!ltr_test_connection()) {
    Serial.println("LTR329 not responding!");
    return;
  }
  
  // Read Part ID to confirm it's the right sensor
  uint8_t part_id = ltr_read_part_id();
  if (part_id != 0xA0) {
    Serial.println("Wrong Part ID - not LTR329!");
    return;
  }

  // Configure Gain - CRITICAL: Must set mode bit to activate!
  Wire1.beginTransmission(LTR_ADDRESS);
  Wire1.write(LTR_ALS_CONTR_ADDRESS);
  Wire1.write(ltr->gain | 0x01); // Set bit 0 to 1 to activate
  Wire1.endTransmission();
  delay(10);

  // Configure Measuring Rate
  Wire1.beginTransmission(LTR_ADDRESS);
  Wire1.write(LTR_ALS_MEAS_RATE_ADDRESS);
  Wire1.write(ltr->measureRate);
  Wire1.endTransmission();
  delay(10);

  Serial.println("LTR329 initialized successfully");
}

bool ltr_test_connection() {
  Wire1.beginTransmission(LTR_ADDRESS);
  byte error = Wire1.endTransmission();
  return (error == 0);
}

uint8_t ltr_read_part_id() {
  Wire1.beginTransmission(LTR_ADDRESS);
  Wire1.write(0x86); // PART_ID register
  Wire1.endTransmission();
  
  Wire1.requestFrom(LTR_ADDRESS, 1);
  if (Wire1.available()) {
    return Wire1.read();
  }
  return 0xFF;
}

static uint16_t read_single_channel(const uint8_t low_reg, const uint8_t high_reg) {
  uint8_t lowByte = 0, highByte = 0;
  
  // Read low byte
  Wire1.beginTransmission(LTR_ADDRESS);
  Wire1.write(low_reg);
  Wire1.endTransmission();
  if (Wire1.requestFrom(LTR_ADDRESS, 1) == 1) {
    lowByte = Wire1.read();
  }
  
  // Read high byte
  Wire1.beginTransmission(LTR_ADDRESS);
  Wire1.write(high_reg);
  Wire1.endTransmission();
  if (Wire1.requestFrom(LTR_ADDRESS, 1) == 1) {
    highByte = Wire1.read();
  }
  
  return (highByte << 8) | lowByte;
}

void ltr_read_channel(ltr_t *ltr, const uint8_t *channel){
  uint16_t result = read_single_channel(channel[0], channel[1]);
  
  Serial.print("Registers 0x");
  Serial.print(channel[0], HEX);
  Serial.print(", 0x");
  Serial.print(channel[1], HEX);
  Serial.print(": Value = ");
  Serial.println(result);

  if (channel[0] == ch0[0]) {
    ltr->channel0 = result;
  } 
  else if (channel[0] == ch1[0]) {
    ltr->channel1 = result;
  }
}