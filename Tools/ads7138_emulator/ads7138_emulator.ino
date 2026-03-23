// roughly emulates basic TI ADS7138IRTER functionality with 
// mock values (000, 111, 222, ..., 777) for testing 

// TODO: emulate temperature values....

#include <Wire.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define ADS7138_ADDR      0x10
#define REG_SYSTEM_STATUS 0x00
#define REG_CHANNEL_SEL   0x11

// Some opcodes
#define OPCODE_WRITE_SINGLE 0x08
#define OPCODE_READ_SINGLE  0x10
#define OPCODE_GENERAL_CMD  0x00   

// Simple register map 
uint8_t regs[0x20];

// --- Configuration ---
const uint8_t PWM_IN_PIN = 15; // GP15
const uint8_t LED_PIN = 25;    

// State for next I2C read
volatile bool   nextIsRegRead = false;
volatile uint8_t currentReg   = 0;
volatile uint8_t currentChan  = 0;  // 0–7, manual mode

struct repeating_timer timer;

// Dummy ADC values per channel (12-bit)
uint16_t dummyValueForChannel(uint8_t ch) {
  ch &= 0x07;
  // 0x000, 0x111, 0x222, ... 0x777 (all fit in 12 bits)
  return (uint16_t)(ch * 0x111) & 0x0FFF;
}

uint8_t readRegister(uint8_t reg) {
  if (reg == REG_SYSTEM_STATUS) {
    // Reset value from datasheet snippet: 0x81
    return 0x81;
  }
  if (reg < sizeof(regs)) {
    return regs[reg];
  }
  return 0x00;  // undefined regs -> 0
}

void writeRegister(uint8_t reg, uint8_t value) {
  if (reg == REG_SYSTEM_STATUS) {
    return;
  }
  if (reg < sizeof(regs)) {
    regs[reg] = value;
  }
  if (reg == REG_CHANNEL_SEL) {
    currentChan = value & 0x07;  // MANUAL_CHID low 3 bits
  }
}

// Called when master sends data to us
void onReceiveHandler(int len) {
  if (len <= 0) return;

  uint8_t first = Wire.read();
  len--;

  // Default: assume next read is a conversion read
  nextIsRegRead = false;

  if (first == OPCODE_GENERAL_CMD) {
    // General command 0x00 -> just ACK & ignore
    return;
  }

  if (first == OPCODE_WRITE_SINGLE && len >= 2) {
    uint8_t reg  = Wire.read();
    uint8_t data = Wire.read();
    writeRegister(reg, data);
    return;
  }

  if (first == OPCODE_READ_SINGLE && len >= 1) {
    currentReg    = Wire.read();
    nextIsRegRead = true;
    return;
  }

  // Fallback: treat first byte as register address (no-opcode style)
  currentReg    = first;
  nextIsRegRead = true;

  // If there's another byte, treat it as a plain write (reg, data)
  if (len >= 1) {
    uint8_t data = Wire.read();
    writeRegister(currentReg, data);
  }
}

// Called when master requests data from us
void onRequestHandler() {
  if (nextIsRegRead) {
    uint8_t val = readRegister(currentReg);
    Wire.write(val);
  } else {
    uint16_t val = dummyValueForChannel(currentChan); 
    // 12-bit value left-aligned into two bytes: D11..D4, D3..D0 xxxx
    uint8_t msb = (val >> 4) & 0xFF;
    uint8_t lsb = (val << 4) & 0xF0;
    Wire.write(msb);
    Wire.write(lsb);
  }
}

void setup() {
  pinMode(PWM_IN_PIN, INPUT);

  // Initialize minimal register defaults
  for (uint8_t i = 0; i < sizeof(regs); ++i) {
    regs[i] = 0x00;
  } 
  regs[REG_SYSTEM_STATUS] = 0x81;
  regs[REG_CHANNEL_SEL]   = 0x00;
  currentChan             = 0;

  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin(ADS7138_ADDR);   // I2C slave at 0x10
  Wire.onReceive(onReceiveHandler);
  Wire.onRequest(onRequestHandler);
}

void loop() {
}
