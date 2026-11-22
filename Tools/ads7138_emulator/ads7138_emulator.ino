#include <Wire.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define ADS7138_ADDR      0x10
#define REG_SYSTEM_STATUS 0x00
#define REG_CHANNEL_SEL   0x11

// Opcodes (subset)
#define OPCODE_WRITE_SINGLE 0x08
#define OPCODE_READ_SINGLE  0x10
#define OPCODE_GENERAL_CMD  0x00   // NOP in our stub

// Simple register map (0x00–0x1F is enough for this stub)
uint8_t regs[0x20];
uint16_t channels[8];

// --- Configuration ---
const uint8_t PWM_IN_PIN = 15; // GP15
const uint8_t LED_PIN = 25;    // Built-in LED
const int INTERVAL_MS = 50;    // Run simulation every 50ms

// true: return the fixed temperatures below; false: run GP15 heating/cooling.
const bool FIXED_TEMPERATURE_MODE = true;
// Degrees Celsius per ADC channel; adjacent pairs drive heaters 1 through 4.
// With BMS thresholds 5 C / 15 C: 0 C turns ON, 10 C holds, 20 C turns OFF.
const float TEST_TEMPERATURES_C[8] = {
  0.0f, 0.0f,    // Heater 1: cold -> ON
  20.0f, 20.0f,  // Heater 2: warm -> OFF
  10.0f, 10.0f,  // Heater 3: in-band -> retain state (OFF after BMS reset)
  0.0f, 0.0f     // Heater 4: sensor 8 is overridden below -> retain state
};
// Fixed mode only: true forces ADC code 0 (invalid for the logarithm).
// This tests invalid sensor data while I2C transfers still succeed.
const bool TEST_INVALID_READINGS[8] = {
  false, false,  // Heater 1: sensors 1 and 2
  false, false,  // Heater 2: sensors 3 and 4
  false, false,  // Heater 3: sensors 5 and 6
  false, true    // Heater 4: sensor 7 valid, sensor 8 invalid
};

// --- Thermistor Simulation Constants ---
// Honeywell 112-103FAJ-B01; match heater_controller.h if the sensor changes.
const float THERMISTOR_NOMINAL = 10000; // Resistance at 25 degrees C (10k)
const float TEMPERATURE_NOMINAL = 25;   // Temp. for nominal resistance (almost always 25 C)
const float BETA_COEFFICIENT = 3669;    // Beta (kelvin), specified at 25/85 C
const float SERIES_RESISTOR = 10000;    // The value of the 'other' resistor in the divider (10k)

// --- Global Simulation Variables ---
volatile float temperature = 20.0; 

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
    // Read-only in this stub
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
  if (len <= 0) {
    return;
  }

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
    uint16_t val = channels[currentChan];
    // uint16_t val = dummyValueForChannel(currentChan); 

    // 12-bit value left-aligned into two bytes: D11..D4, D3..D0 xxxx
    uint8_t msb = (val >> 4) & 0xFF;
    uint8_t lsb = (val << 4) & 0xF0;
    Wire.write(msb);
    Wire.write(lsb);
  }
}

bool isHeatingActive() {
  return digitalRead(PWM_IN_PIN) == HIGH;
}

// --- Helper: Convert Temp to Simulated ADC Value ---
// This reverses the standard thermistor reading logic.
// Returns a value 0-4095 (12-bit ADC)
uint16_t temperatureToADC(float tempC) {
    // 1. Convert Celsius to Kelvin
    float tempK = tempC + 273.15;
    float tempRefK = TEMPERATURE_NOMINAL + 273.15;

    // 2. Calculate Thermistor Resistance (Steinhart-Hart / Beta equation)
    // R = R0 * exp(B * (1/T - 1/T0))
    float resistance = THERMISTOR_NOMINAL * exp(BETA_COEFFICIENT * (1.0/tempK - 1.0/tempRefK));

    // 3. Simulate Voltage Divider (Vcc -> SeriesR -> [ADC] -> Thermistor -> GND)
    // This is the most common config. 
    // Voltage Fraction = R_therm / (R_series + R_therm)
    float voltageFraction = resistance / (SERIES_RESISTOR + resistance);

    // 4. ADS7138 uses 4096 counts per full scale; maximum code is 4095.
    float adcVal = voltageFraction * 4096.0;

    // Clamp values
    if (adcVal > 4095) {
      return 4095;
    }
    if (adcVal < 0) {
      return 0;
    }
    return (uint16_t)adcVal;
}

void updateChannels() {
  float currentTemp = temperature;
  for (uint8_t i = 0; i < 8; i++) {
    if (FIXED_TEMPERATURE_MODE && TEST_INVALID_READINGS[i]) {
      channels[i] = 0u;
    } else {
      channels[i] = temperatureToADC(
          FIXED_TEMPERATURE_MODE ? TEST_TEMPERATURES_C[i] : currentTemp);
    }
  }
}

// --- Timer Interrupt Service Routine (ISR) ---
bool timer_callback(struct repeating_timer *t) {
    // Determine heating vs cooling
    // Simple simulation: HIGH = Heating, LOW = Cooling
    if (digitalRead(PWM_IN_PIN) == HIGH) {
        temperature += 0.1; 
        // Serial.println("Got pwm data");
    } else {
        temperature -= 0.05; 
    }

    // Safety bounds for simulation physics
    if (temperature < -200) {
      temperature = -200;
    }
    if (temperature > 500) {
      temperature = 500;
    }

    return true; 
}

void setup() {
  pinMode(PWM_IN_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);

  if (!FIXED_TEMPERATURE_MODE) {
    add_repeating_timer_ms(-INTERVAL_MS, timer_callback, NULL, &timer);
  }

  // Have valid readings ready before the first master request.
  updateChannels();

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

  // Enable general call ACK (address 0x00) on AVR-based boards
#if defined(TWAR)
  TWAR |= 0x01;  // Set TWGCE bit: respond to general call address 0x00
#endif
}

void loop() {
  static unsigned long lastPrint = 0;
    
    // Update data output every 100ms
    if (millis() - lastPrint > 100) {
        lastPrint = millis();
        
        updateChannels();

        // Log actual channel codes, including when fixed mode is selected.
        for (uint8_t i = 0; i < 8; i++) {
          Serial.print(channels[i]);
          Serial.print(' ');
        }
        Serial.println();
      
      
        // Visual heartbeat
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
}
