#include <SPI.h>
#include "./LittleFS/lfs.h"
#include "./LittleFS/lfs_util.h"

/*
Pico Pin# ---> MRAM Pin#
MOSI: 25 ---> 5
MISO: 21 ---> 2
VCC: 36 ---> 8
VSS: 38 ---> 4
SCK: 24 ---> 6
CSn: 22 ---> 1
WPn: 26 ---> 3

Pull-Up Resistor used from VCC to CSn and WPn using 10k ohm resistor
VCC ---> 10k ---> WPn / CSn
*/

////////////////////////////////////////////////////////////////////// MACROS

#define MISO_PIN 16
#define CSn_PIN 17
#define SCK_PIN 18
#define MOSI_PIN 19
#define WPn_PIN 20

// S3A3204V0M MRAM Command Definitions


//Commands not needing ADDR or DATA
#define Write_EN 0x06  // Command to Write Memory Enable (typical for MRAM)
#define Write_DIS 0x04  // Command to Write Memory Disable (typical for MRAM)
#define SSPI_EN 0xFF
#define Entr_Deep_Pwr_Mode 0xB9
#define Exit_Deep_Pwr_Mode 0xAB
#define Software_Reset_EN 0x66
#define Software_Reset 0x99
#define ADDRESS_SIZE_IN_BYTES 3

//Commands needing Data
#define Read_Status_Reg 0x05
#define Read_Serial_Reg 0xC3
#define READ_DEVICE_ID_CMD 0x9F  // Command to read device ID (typical for MRAM)
#define READ_UNIQUE_ID_CMD 0x4C  // Command to read unique ID (typical for MRAM)
#define Write_Status_Reg 0x01

//Commands needing Data + Address
#define Read_Any_Reg 0x65
#define Write_Any_Reg 0x71
#define Read_Mem_Array 0x03
#define Write_Mem_Array 0x02



const uint64_t STATUS_REG = 0x000000;
const uint64_t CONFIGURATION_REG1 = 0x000002;
const uint64_t CONFIGURATION_REG2 = 0x000003;
const uint64_t CONFIGURATION_REG3 = 0x000004;
const uint64_t CONFIGURATION_REG4 = 0x000005;
const uint64_t DEVICE_ID_REG = 0x000030;
const uint64_t UNIQUE_ID_REG = 0x000040;
const uint64_t SERIAL_NUM_REG = 0x000080;
const uint64_t NON_AUGMENTED_REG = 0x0002000;

///////LFS CONFIG

/*
lfs_t lfs;
lfs_file_t file;


//This breaks something. How? good question
int lfs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size){
  uint32_t addr = NON_AUGMENTED_REG + (block * c->block_size) + off;
  MRAMread_cmd_addr(Read_Any_Reg, addr, buffer, size);
    //in littlefs example code, they use memcpy()
}

int lfs_write(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size){
  uint32_t addr = NON_AUGMENTED_REG + (block * c->block_size) + off;
  MRAMwrite_cmd_addr_data(Write_Any_Reg, addr, buffer, size);
    //in littlefs example code, they use memcpy()
}

int lfs_erase(const struct lfs_config *c, lfs_block_t block){
  return 0;
}

int lfs_sync(const struct lfs_config *c){
  return 0;
}

struct lfs_config cfg = { 
  NULL, //Optional
  lfs_read, //.read
  lfs_write,  //.prog
  lfs_erase,  //.erase
  lfs_sync, //.synce
  1,  //.read_size
  1,  //.write_size
  512,  //.block_size
  8192, //.block_count
  -1, //.block_cycle
  64, //.cache_size
  16, //.lookahead_size
  NULL, //Optional
  NULL, //Optional
  NULL, //Optional
  NULL, //Optional
  NULL, //Optional
  NULL, //Optional
  NULL, //Optional
  NULL, //Optional
  NULL  //Optional
};

*/


//Global Classes for SPI settings
SPISettings spiSettings(1000000, MSBFIRST, SPI_MODE0);
arduino::MbedSPI SPI0(MISO_PIN, MOSI_PIN, SCK_PIN);

////////////////////////////////////////////////////////////////////// MAIN LOOP

void setup() {
  // put your setup code here, to run once:

  //Init LittleFS
  /*
  int err = lfs_mount(&lfs, &cfg);
  if(err){
    lfs_format(&lfs, &cfg);
    lfs_mount(&lfs, &cfg);
  }
  */

  Serial.begin(9600);
  Serial.println("Start");
  delay(100);

  initMRAM();

}

void loop() {
  // put your main code here, to run repeatedly:
  
  uint8_t readSerialNumber[8];
  uint8_t writeSerialNumber[8] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};

  MRAMwrite_cmd_data(0xC2, writeSerialNumber, sizeof(writeSerialNumber) / sizeof(uint8_t));

  Serial.print("Device ID: ");

  MRAMread_cmd(Read_Serial_Reg, readSerialNumber, sizeof(readSerialNumber)/sizeof(uint8_t));

  for(int i = 0; i < 8; i++){
  Serial.print(readSerialNumber[i], HEX);
  }
  Serial.println();

  delay(1000);
  
  /*
  uint32_t boot_count = 0;
  
  lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
  lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

  // update boot count
  boot_count += 1;
  lfs_file_rewind(&lfs, &file);
  lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

  // remember the storage is not updated until the file is closed successfully
  lfs_file_close(&lfs, &file);

  Serial.print("boot count: ");
  Serial.println(boot_count);
  delay(1000);
  */
  }


////////////////////////////////////////////////////////////////////// UTILITY FUNCTIONS
/////////////INIT

void configWP(void){
  /*
  1. Send WREN command to MRAM
  2. Turn Modify status/config register to liking
  3. Protect status/config register (either state 1 or 2 in Table 14)
  */
  WREN_cmd();

  //Don't need to do anything with status register, but implementing a function
  //would be necessary if it needs to be

 //Don't need to do anything with the config register, but implementing a function
 //would be necessary if it needs to be
}

void WREN_cmd(void){
  SPI0.beginTransaction(spiSettings);
  digitalWrite(CSn_PIN, LOW);

  SPI0.transfer(Write_EN);

  digitalWrite(CSn_PIN, HIGH);
  SPI0.endTransaction();
}

void WRDI_cmd(void){
  SPI0.beginTransaction(spiSettings);
  digitalWrite(CSn_PIN, LOW);

  SPI0.transfer(Write_DIS);

  digitalWrite(CSn_PIN, HIGH);
  SPI0.endTransaction();
}

void initMRAM(void){
  pinMode(CSn_PIN, OUTPUT);
  pinMode(WPn_PIN, OUTPUT);
  digitalWrite(CSn_PIN, HIGH); //Upon startup, CSn should follow VCC for data protection

  SPI0.begin();
  configWP();

}



/////////////WRITE

void MRAMwrite_cmd_data(uint8_t cmd, uint8_t* data, int arraySize){
  //Write to MRAM with command that takes only Data without needing Address
  WREN_cmd();
  SPI0.beginTransaction(spiSettings);
  digitalWrite(CSn_PIN, LOW);

  SPI0.transfer(cmd); //Give instruction, followed by data
  for(int i = 0; i < arraySize; i++){
    //Serial.print("Sending: ");
    //Serial.println(data[i], HEX);
    SPI0.transfer(data[i]);
  }

  digitalWrite(CSn_PIN, HIGH);
  SPI0.endTransaction();
  WRDI_cmd();
}



void MRAMwrite_cmd_addr_data(uint8_t cmd, uint32_t addr, const void* data, lfs_size_t size){
  //Write to MRAM data housed at a specified Address

  //Retrieve 24 LSB
  // uint8_t addr_byte[] = {(addr & 0x000F0000) >> 16, (addr & 0x0000FF00) >> 8, (addr & 0x000000FF)};
  uint8_t addr_byte[] = {(addr & 0xF00) >> 16, (addr & 0xF0) >> 8, (addr & 0xF)};
  //uint8_t addr_byte2 = (address & 0x00FF0000) >> 16;
  //uint8_t addr_byte1 = (address & 0x0000FF00) >> 8;
  //uint8_t addr_byte0 = (address & 0x000000FF);  

  WREN_cmd();
  SPI0.beginTransaction(spiSettings);
  digitalWrite(CSn_PIN, LOW);

  SPI0.transfer(cmd); //Give instruction, then address, followed by data
  delayMicroseconds(1); //Ensure chip received command
  for(int i = 0; i < 2; i++){
    SPI0.transfer(addr_byte[i]);
  }
  delayMicroseconds(1);
  for(int i = 0; i < size; i++){
    SPI0.transfer(*((uint32_t*)data + i)); //*void -> *int type cast required || Needs to be translated for remaining functions, once littlefs is resolved
  }
  delayMicroseconds(1);

  digitalWrite(CSn_PIN, HIGH);
  SPI0.endTransaction();
  WRDI_cmd();
}

/////////////READ
void MRAMread_cmd(uint8_t cmd, uint8_t* data, int arraySize){
  //Read MRAM data where command does not require specific address

  SPI0.beginTransaction(spiSettings);
  digitalWrite(CSn_PIN, LOW);

  SPI0.transfer(cmd);
  //delayMicroseconds(1);//Ensure chip received command
  //data = SPI0.transfer(0x00); //Dummy value to retrieve data
  for(int i = 0; i < arraySize; i++){
    data[i] = SPI0.transfer(0x00); //Dummy value to retrieve data
    //Serial.print("READING: ");
    //Serial.println(data[i], HEX);
  }

  digitalWrite(CSn_PIN, HIGH);
  SPI0.endTransaction();

}

void MRAMread_cmd_addr(uint8_t cmd, uint32_t addr, void* data, lfs_size_t size){
  //Read MRAM data where command does require a specified address
  //Retrieve 24 LSB
  //uint8_t addr_byte[] = {(addr & 0x00FF0000) >> 16, (addr & 0x0000FF00) >> 8, (addr & 0x000000FF)};
  uint8_t addr_byte[] = {(addr & 0xF00) >> 16, (addr & 0xF0) >> 8, (addr & 0xF)};

  //uint8_t addr_byte2 = (address & 0x00FF0000) >> 16;
  //uint8_t addr_byte1 = (address & 0x0000FF00) >> 8;
  //uint8_t addr_byte0 = (address & 0x000000FF);  

  SPI0.beginTransaction(spiSettings);
  digitalWrite(CSn_PIN, LOW);

  SPI0.transfer(cmd);
  delayMicroseconds(1); //Ensure chip received command
  for(int i = 0; i < 3; i++){
    SPI0.transfer(addr_byte[i]);
  }
  delayMicroseconds(1);
  //data = SPI0.transfer(0x00); //Dummy value to retrieve data
  for(int i = 0; i < size; i++){
    *((uint32_t*)data + i) = SPI0.transfer(0x00); //*void -> *int type cast required || Needs to be translated for remaining functions, once littlefs is resolved
  }

  digitalWrite(CSn_PIN, HIGH);
  SPI0.endTransaction();
}



//////////////////////////////////////LEGACY CODE

/*
void MRAM_writeTo(int command, int address, int data){
  WREN_cmd();
  SPI0.beginTransaction(spiSettings);
  digitalWrite(CSn_PIN, LOW);

  uint8_t addr_byte2 = (address & 0x00FF0000) >> 16;
  uint8_t addr_byte1 = (address & 0x0000FF00) >> 8;
  uint8_t addr_byte0 = (address & 0x000000FF);  

  SPI0.transfer(command);
  SPI0.transfer(addr_byte2);
  SPI0.transfer(addr_byte1);
  SPI0.transfer(addr_byte0);
  SPI0.transfer(data);

  digitalWrite(CSn_PIN, HIGH);

  SPI0.endTransaction();
  WRDI_cmd();
}

uint8_t MRAM_read(int command, int address){
  uint8_t MRAM_output;
  WREN_cmd();
  SPI0.beginTransaction(spiSettings);
  digitalWrite(CSn_PIN, LOW);

  uint8_t addr_byte2 = (address & 0x00FF0000) >> 16;
  uint8_t addr_byte1 = (address & 0x0000FF00) >> 8;
  uint8_t addr_byte0 = (address & 0x000000FF);  

  SPI0.transfer(command);
  SPI0.transfer(addr_byte2);
  SPI0.transfer(addr_byte1);
  SPI0.transfer(addr_byte0);
  MRAM_output = SPI0.transfer(0x00); //Dummy value to retrieve data

  digitalWrite(CSn_PIN, HIGH);

  SPI0.endTransaction();
  WRDI_cmd();
  return MRAM_output;
}
/*
// Read a region in a block. Negative error codes are propagated
// to the user.
int read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) {
  uint32_t addr = (c->block_size * block) + off;
  uint8_t* data = (uint8_t*)buffer;
  for (size_t i = addr; i < addr + size; ++i) {
    data[i] = MRAM_read(Read_Memory_Array, addr + i);
  }
}

*/

