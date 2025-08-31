// This code sends a command (0xFF) to the slave device at address 0x08 over I2C,
// waits for a byte of data in return, and prints the received byte in hexadecimal
// to the Serial Monitor. This process repeats every second.


#include <Wire.h>

const static uint8_t crc_init_value = 0xFF;

// Value XORed to the final register before the CRC is returned
const static uint8_t crc_xor_value = 0xFF;

// Pre-computed AUTOSAR CRC8 table
const static uint8_t crc_lookup_table[256] = {
        0x00, 0x2F, 0x5E, 0x71, 0xBC, 0x93, 0xE2, 0xCD, 0x57, 0x78, 0x09, 0x26, 0xEB, 0xC4, 0xB5, 0x9A,
        0xAE, 0x81, 0xF0, 0xDF, 0x12, 0x3D, 0x4C, 0x63, 0xF9, 0xD6, 0xA7, 0x88, 0x45, 0x6A, 0x1B, 0x34,
        0x73, 0x5C, 0x2D, 0x02, 0xCF, 0xE0, 0x91, 0xBE, 0x24, 0x0B, 0x7A, 0x55, 0x98, 0xB7, 0xC6, 0xE9,
        0xDD, 0xF2, 0x83, 0xAC, 0x61, 0x4E, 0x3F, 0x10, 0x8A, 0xA5, 0xD4, 0xFB, 0x36, 0x19, 0x68, 0x47,
        0xE6, 0xC9, 0xB8, 0x97, 0x5A, 0x75, 0x04, 0x2B, 0xB1, 0x9E, 0xEF, 0xC0, 0x0D, 0x22, 0x53, 0x7C,
        0x48, 0x67, 0x16, 0x39, 0xF4, 0xDB, 0xAA, 0x85, 0x1F, 0x30, 0x41, 0x6E, 0xA3, 0x8C, 0xFD, 0xD2,
        0x95, 0xBA, 0xCB, 0xE4, 0x29, 0x06, 0x77, 0x58, 0xC2, 0xED, 0x9C, 0xB3, 0x7E, 0x51, 0x20, 0x0F,
        0x3B, 0x14, 0x65, 0x4A, 0x87, 0xA8, 0xD9, 0xF6, 0x6C, 0x43, 0x32, 0x1D, 0xD0, 0xFF, 0x8E, 0xA1,
        0xE3, 0xCC, 0xBD, 0x92, 0x5F, 0x70, 0x01, 0x2E, 0xB4, 0x9B, 0xEA, 0xC5, 0x08, 0x27, 0x56, 0x79,
        0x4D, 0x62, 0x13, 0x3C, 0xF1, 0xDE, 0xAF, 0x80, 0x1A, 0x35, 0x44, 0x6B, 0xA6, 0x89, 0xF8, 0xD7,
        0x90, 0xBF, 0xCE, 0xE1, 0x2C, 0x03, 0x72, 0x5D, 0xC7, 0xE8, 0x99, 0xB6, 0x7B, 0x54, 0x25, 0x0A,
        0x3E, 0x11, 0x60, 0x4F, 0x82, 0xAD, 0xDC, 0xF3, 0x69, 0x46, 0x37, 0x18, 0xD5, 0xFA, 0x8B, 0xA4,
        0x05, 0x2A, 0x5B, 0x74, 0xB9, 0x96, 0xE7, 0xC8, 0x52, 0x7D, 0x0C, 0x23, 0xEE, 0xC1, 0xB0, 0x9F,
        0xAB, 0x84, 0xF5, 0xDA, 0x17, 0x38, 0x49, 0x66, 0xFC, 0xD3, 0xA2, 0x8D, 0x40, 0x6F, 0x1E, 0x31,
        0x76, 0x59, 0x28, 0x07, 0xCA, 0xE5, 0x94, 0xBB, 0x21, 0x0E, 0x7F, 0x50, 0x9D, 0xB2, 0xC3, 0xEC,
        0xD8, 0xF7, 0x86, 0xA9, 0x64, 0x4B, 0x3A, 0x15, 0x8F, 0xA0, 0xD1, 0xFE, 0x33, 0x1C, 0x6D, 0x42 };

uint8_t TINYPROTOCOL_CalculateCRC(const uint8_t* buffer, uint8_t buffer_size) {
    uint8_t crc = crc_init_value;

    uint8_t i;
    for (i = 0; i < buffer_size; i++) {
        crc = crc_lookup_table[buffer[i] ^ crc];
    }

    return crc ^ crc_xor_value;
}

// In general to send telerequest commands it's 0x80 | <command id> then tinyprotocol extracts with & 0x7F = 0b0111111
//                0x80 = 0b10000000
// Example command = 1 = 0b00000001
// Or together:          0b10000001 = 0x81
// Extract:  0x7F & 0x81 0b01111111
//                       0b00000001 // which is our example command id
#define BMS_SYSTEM_STATUS_ID 0x81
#define BMS_FLAG_ID 0x83
#define BMS_CURRENT_DRAW_ID 0x84
#define BMS_CURRENT_CHARGE_ID 0x85
#define BMS_VOLTAGE_BATTERY1_ID 0x86
#define BMS_VOLTAGE_BATTERY2_ID 0x87
#define BMS_VOLTAGE_COMBINED_ID 0x88
#define BMS_THERMISTOR03_DATA_ID 0x8A
#define BMS_THERMISTOR47_DATA_ID 0x8B
#define BMS_HEATER_CONTROLLER_ID 0x09 // Telecommands do not have 0x80 in front!

#define PWM_PIN 15

void sendTeleCommand(uint8_t cmd_id, const uint8_t* buff, uint8_t size)
{
  uint8_t cpy[size + 1] = {};
  cpy[0] = cmd_id;
  memcpy(&cpy[1], buff, size);
  const uint8_t crc = TINYPROTOCOL_CalculateCRC(cpy, size + 1);
  Wire.beginTransmission(0x08);
  Wire.write(0x9b);   // MAGIC
  Serial.print("Sending CMD: ");
  Serial.println(cmd_id, HEX);
  Wire.write(cmd_id);
  Serial.print("Sending Data: ");
  for (uint8_t i = 0; i < size; ++i) {
    Wire.write(buff[i]);
    Serial.print(buff[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("Sending CRC: ");
  Serial.print(crc, HEX);
  Serial.println();
  Wire.write(crc);
  Wire.endTransmission(false);
}

void sendPWMData() {
  uint8_t buff[4] = {75, 50, 98, 5};
  Serial.print("Sending PWM data: ");
  for (size_t i = 0; i < 4; ++i) {
    Serial.print(" ");
    Serial.print(buff[i], HEX);
  }
  Serial.println();
  sendTeleCommand(BMS_HEATER_CONTROLLER_ID, buff, sizeof(buff));
}

void requestFlags() {
  uint8_t buff[2] = {};
  sendTeleChannelRequest(BMS_FLAG_ID, buff, sizeof(buff));
  Serial.print("buffer: ");
  Serial.print(buff[0], BIN);
  Serial.println(buff[1], BIN);
}

void printValue(byte* buffer) {
  size_t length = 4;
  int val1 = (buffer[1] << 8) | buffer[0];
  int val2 = (buffer[3] << 8) | buffer[2];
  Serial.print(val1);
  Serial.print(" ");
  Serial.print(val2);
  Serial.println();
}

void printConvertedADCValue(byte* buffer) {
  int val1 = (buffer[1] << 8) | buffer[0];
  int val2 = (buffer[3] << 8) | buffer[2];
  Serial.print(val1 * 3.3 / 4096);
  Serial.print(" ");
  Serial.print(val2 * 3.3 / 4096);
  Serial.println();
}

void requestADC() {
  Serial.println("################### ADC data begin ##############################");
  uint8_t buff[4] = {};  

  sendTeleChannelRequest(BMS_CURRENT_DRAW_ID, buff, sizeof(buff));
  Serial.println("Current Draw: ");
  Serial.print("Raw ADC value 1 and 2: ");
  printValue(buff);
  Serial.print("ADC value * 3.3 / 4096 (V): ");
  printConvertedADCValue(buff);
  
  sendTeleChannelRequest(BMS_CURRENT_CHARGE_ID, buff, sizeof(buff));
  Serial.println("Current CHARGE: ");
  Serial.print("Raw ADC value 1 and 2: ");
  printValue(buff);
  Serial.print("ADC value * 3.3 / 4096 (V): ");
  printConvertedADCValue(buff);


  sendTeleChannelRequest(BMS_VOLTAGE_BATTERY1_ID, buff, sizeof(buff));
  Serial.println("Voltage Battery 1:");
  Serial.print("Raw ADC value 1 and 2: ");
  printValue(buff);
  Serial.print("ADC value * 3.3 / 4096 (V): ");
  printConvertedADCValue(buff);

  sendTeleChannelRequest(BMS_VOLTAGE_BATTERY2_ID, buff, sizeof(buff));
  Serial.println("Voltage Battery 2:");
  Serial.print("Raw ADC value 1 and 2: ");
  printValue(buff);
  Serial.print("ADC value * 3.3 / 4096 (V): ");
  printConvertedADCValue(buff);

  sendTeleChannelRequest(BMS_VOLTAGE_COMBINED_ID, buff, sizeof(buff));
  Serial.println("Voltage Combined:");
  Serial.print("Raw ADC value 1 and 2: ");
  printValue(buff);
  Serial.print("ADC value * 3.3 / 4096 (V): ");
  printConvertedADCValue(buff);

  Serial.println("################### adc data end ##############################");
}

void sendTeleChannelRequest(uint8_t channel_id, uint8_t* buff, size_t len) {
  
  // Begin transmission to slave at address 0x08
  Wire.beginTransmission(0x08);
  Wire.write(0x9b);   // MAGIC

  // FIXME: This is really stupid, this function should definitely expect unaltered channel_id/
  //  As it stands it is expecting channel_id | 0x80
  uint8_t cmd_id = channel_id & 0x7F;
  Serial.print(" sending cmd: cmd id: ");
  Serial.print(cmd_id, HEX);
  Serial.print(", channel_id: ");
  Serial.println(channel_id, HEX);
  Wire.write(channel_id);   // Test telemetry request command
  const uint8_t crc = TINYPROTOCOL_CalculateCRC(&cmd_id, 1);
  Serial.print("CRC: ");
  Serial.print(crc);
  Serial.println();
  Wire.write(crc); 

  if (Wire.endTransmission(false) == 0) {
    // Serial.println("ACK!");
  }

  Wire.requestFrom(0x8, len, true);    // read
  for (size_t i = 0; i < len; i++) {
    buff[i] = Wire.read();
  }
}

void adcHalfChannelHelper(uint8_t channel_id, uint8_t* buffer, size_t len) {
  uint8_t buffer_idx = 0;
  sendTeleChannelRequest(channel_id, buffer, len);
  for (size_t i = 0; i < len; ++i) {
    Serial.print("ADC raw: ");
    Serial.print(i);
    Serial.print(" ");
    uint16_t val = 0;
    val = (buffer[buffer_idx] << 8) | (buffer[buffer_idx + 1] & 0xff);
    buffer_idx += 2;
    Serial.print(val, HEX);
    Serial.print(" converted: ");
    Serial.println(((float)val * 3.3) / (1 << 12));
  }
}

void getExtADC() {
  Serial.println("################### External ADC data begin ##############################");
  size_t len = 8;
  uint8_t buff03[len] = {};
  uint8_t buff47[len] = {};  
  adcHalfChannelHelper(BMS_THERMISTOR03_DATA_ID, buff03, len);
  adcHalfChannelHelper(BMS_THERMISTOR47_DATA_ID, buff47, len);
  Serial.println("################### External ADC data end ##############################");
}

void setup() {
  Wire.setSDA(4);
  Wire.setSCL(5);
  // pinMode(15, INPUT);
  // pinMode(1, OUTPUT);
  // digitalWrite(1, HIGH);
  Wire.begin();         // Initialize I2C
  Serial.begin(115200);   // Start Serial Monitor
  // while (!Serial);      // Wait for Serial Monitor to open (for some boards)
}

void loop() {
  // requestADC();            // Send command and read data
  // Serial.println("Requesting Flags...");
  // requestFlags();
  // sendPWMData();
  getExtADC();
  delay(1000);          // Wait 1 second before repeating
  // unsigned long highTime = pulseIn(PWM_PIN, HIGH);
  // unsigned long lowTime = pulseIn(PWM_PIN, LOW);
  // unsigned long period = highTime + lowTime;
  // Serial.print("HighTime: "); 
  // Serial.println(highTime);
  // Serial.println("LowTime: ");
  // Serial.println(lowTime);
  // Serial.print("Period: ");
  // Serial.println(period);
  // if (period > 0) {
  //   float dutyCycle = 100.0 * highTime / period;
  //   Serial.print("Duty Cycle: ");
  //   Serial.println(dutyCycle);  // percent (0–100%)
  // }
  // delay(100);
}



void printBufferInHex(byte* buffer, size_t length) {
  for (size_t i = 0; i < length; i++) {
    if (buffer[i] < 0x10) {
      Serial.print("0"); // Add leading zero for single digit hex values
    }
    Serial.print(buffer[i], HEX); // Print each byte in hex
    Serial.print(" "); // Add a space between bytes
  }
  Serial.println(); // New line after printing the buffer
}