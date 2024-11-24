#include <SPI.h>

extern "C" {
  #include "lib/lfs.h"
  #include "lib/lfs_util.h"
}

// Pin Definitions for the Raspberry Pi Pico (Custom SPI Pins)
#define CS_PIN 1    // Chip select pin (CS) -> GP1
#define SCK_PIN 2   // Clock pin (SCK) -> GP2
#define MISO_PIN 0  // MISO (Master In Slave Out) -> GP0
#define MOSI_PIN 3  // MOSI (Master Out Slave In) -> GP3

// S3A3204V0M MRAM Command Definitions
#define READ_DEVICE_ID_CMD 0x9F  // Command to read device ID (typical for MRAM)
#define READ_UNIQUE_ID_CMD 0x4C  // Command to read unique ID (typical for MRAM)
#define Read_Memory_Array 0x03  // Command to Read Memory Array (typical for MRAM)
#define Write_Memory_Array 0x02  // Command to Write Memory Array (typical for MRAM)
#define Write_Memory_Enable 0x06  // Command to Write Memory Enable (typical for MRAM)
#define Write_Memory_Disable 0x04  // Command to Write Memory Disable (typical for MRAM)
#define ADDRESS_SIZE_IN_BYTES 3

// SPI settings
SPISettings spiSettings(1000000, MSBFIRST, SPI_MODE0);

arduino::MbedSPI SPI0(MISO_PIN, MOSI_PIN, SCK_PIN); // Pass custom pins to SPI0.begin


void setup() {
  // Start serial communication
  Serial.begin(115200);
  delay(1000);
  
  // Initialize SPI communication
  SPI0.begin();  
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);  // Deselect MRAM initially

  writeMemoryEn();
}


void loop() {
  uint32_t MemAddress = 0x000000;
  struct lfs_config *c;
  c->block_size = 512;

  uint8_t inbuffer[10] = {1,2,3,4,5,6,7,8,9,10};

  prog(c, 0, MemAddress, inbuffer, 10);
  
  delay(500);

  uint8_t outbuffer[10];
  read(c, 0, MemAddress, outbuffer, 10);
  Serial.print("Read data: ");
  for (size_t i = 0; i < 10; ++i) {
    Serial.print(outbuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  delay(5000);
}

// Read a region in a block. Negative error codes are propagated
// to the user.
int read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) {
  uint32_t addr = (c->block_size * block) + off;
  uint8_t* data = (uint8_t*)buffer;
  for (size_t i = addr; i < addr + size; ++i) {
    data[i] = readMemoryArray(addr + i);
  }
}

// Program a region in a block. The block must have previously
// been erased. Negative error codes are propagated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size) {
  uint8_t* data = (uint8_t*)buffer;
  uint32_t addr = (c->block_size * block) + off;
  for (size_t i = addr; i < addr + size; ++i) {
    writeMemoryEn();
    delay(50);
    writeMemoryArray(addr + i, data[i]);
  }
}


uint32_t readDeviceID() {
  // Set SPI settings (speed, bit order, data mode)
  SPI0.beginTransaction(spiSettings);
  
  Serial.println("Starting MRAM Device ID read...");

  digitalWrite(CS_PIN, LOW);  // Select MRAM device
  
  // Send the Device ID command (usually 0x9F)
  SPI0.transfer(READ_DEVICE_ID_CMD);
  
  // Read the 4-byte response (32-bit Device ID register)
  uint32_t deviceId = 0;
  for (int i = 3; i >= 0; i--) {
    deviceId |= (SPI0.transfer(0x00) << (i * 8));  // Send dummy byte to read each byte of the ID
  }
  
  digitalWrite(CS_PIN, HIGH);  // Deselect MRAM device

  // End SPI transaction
  SPI0.endTransaction();
  
  return deviceId;
}

uint32_t readUniqueID() {
  // Set SPI settings (speed, bit order, data mode)
  SPI0.beginTransaction(spiSettings);
  
  Serial.println("Starting MRAM Unique ID read...");

  digitalWrite(CS_PIN, LOW);  // Select MRAM device
  
  // Send the Device ID command (usually 0x9F)
  SPI0.transfer(READ_UNIQUE_ID_CMD);
  
  // Read the 4-byte response (32-bit Device ID register)
  uint32_t uniqueId = 0;
  for (int i = 3; i >= 0; i--) {
    uniqueId |= (SPI0.transfer(0x00) << (i * 8));  // Send dummy byte to read each byte of the ID
  }
  
  digitalWrite(CS_PIN, HIGH);  // Deselect MRAM device

  // End SPI transaction
  SPI0.endTransaction();
  
  return uniqueId;
}

uint32_t readMemoryArray(uint32_t addr) {
  // Set SPI settings (speed, bit order, data mode)
  SPI0.beginTransaction(spiSettings);
  digitalWrite(CS_PIN, LOW);  // Select MRAM device

  //address
  uint8_t addr_byte2 = (addr & 0x00FF0000) >> 2*8;
  uint8_t addr_byte1 = (addr & 0x0000FF00) >> 1*8;
  uint8_t addr_byte0 = (addr & 0x000000FF);  

  // Transfer command
  SPI0.transfer(Read_Memory_Array);
  // Transfer Address
  SPI0.transfer(addr_byte2);
  SPI0.transfer(addr_byte1);
  SPI0.transfer(addr_byte0);

  uint8_t MemoryArray = SPI0.transfer(0x00);

  digitalWrite(CS_PIN, HIGH);  // Deselect MRAM device

  // End SPI transaction
  SPI0.endTransaction();
  
  return MemoryArray;
}

void writeMemoryArray(uint32_t addr, uint8_t Data) {
  
  // Set SPI settings (speed, bit order, data mode)
  SPI0.beginTransaction(spiSettings);
  digitalWrite(CS_PIN, LOW);  // Select MRAM device
  
  //address
  uint8_t addr_byte2 = (addr & 0x00FF0000) >> 2*8;
  uint8_t addr_byte1 = (addr & 0x0000FF00) >> 1*8;
  uint8_t addr_byte0 = (addr & 0x000000FF);  

  // Transfer command
  SPI0.transfer(Write_Memory_Array);
  // Transfer address
  SPI0.transfer(addr_byte2);
  SPI0.transfer(addr_byte1);
  SPI0.transfer(addr_byte0);
  // transfer 1 byte of data
  SPI0.transfer(Data);

  digitalWrite(CS_PIN, HIGH);  // Deselect MRAM device

  // End SPI transaction
  SPI0.endTransaction();
}

void writeMemoryEn() {
  // Set SPI settings (speed, bit order, data mode)
  SPI0.beginTransaction(spiSettings);
  digitalWrite(CS_PIN, LOW);  // Select MRAM device
  
  SPI0.transfer(Write_Memory_Enable);

  digitalWrite(CS_PIN, HIGH);  // Deselect MRAM device

  // End SPI transaction
  SPI0.endTransaction();
}