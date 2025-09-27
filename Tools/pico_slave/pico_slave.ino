#include "command.h"
#include "LTR.h"

/* Globals */
ltr_t ltr;
packet_t packet;
volatile int received_command = 0;
volatile bool newData = false;
volatile bool lightSensorActive = false;

void setup() {
  /* Initialize Packet */
  packet.newData = false;
  packet.command = 0x00;
  packet.data[0] = 0;
  packet.data[1] = 0;

  /* Initialize Light Sensor */
  ltr.gain = 0x01;
  ltr.measureRate = 0x12;
  ltr.channel1 = 0x00;
  ltr.channel0 = 0x00;
  
  // Begin Connection to Master PICO
  Wire.begin(DEVICE_ADDRESS); // Initialize as I2C slave with address 8
  Wire.onReceive(receiveEvent);  // Register receive event
  Wire.onRequest(requestEvent); // Register request event

  // Begin Connection to Light Sensor
  Wire1.begin();
  Wire1.setClock(100000); // Set to standard 100kHz

  Serial.begin(9600);
  while (!Serial); // Wait for serial monitor
  Serial.println("I2C Slave - Receives and Responds");
  
  Serial.println("Starting LTR329 test...");
  
  // Test before init
  Serial.println("Testing connection...");
  ltr_test_connection();
  ltr_read_part_id();
  delay(100); // Wait for first measurement

  // Initialize LTR
  ltr_init(&ltr);
  delay(1000);
}

void loop() {
  if (newData) {
    Serial.print("Slave received: ");
    Serial.println(received_command);
    packet.newData = false;
  }
}

void receiveEvent(int howMany) {
  Serial.print("How Many : ");
  Serial.println(howMany);
  packet_read(&packet,howMany);
  
  // Check Which Command was received
  if(packet.command == 0x02){ // Read Channel 0
    Serial.println("Read LTR channel 0 ");
    ltr_read_channel(&ltr,ch0);
  }
  else if(packet.command == 0x03){ // Read Channel 1
    Serial.println("Read LTR channel 1 ");
    ltr_read_channel(&ltr,ch1);
  }
  else{
    Serial.print("Not a registered command : ");
    Serial.println(packet.command);
  }
}

void requestEvent() {
  Serial.print("I2C request for command: 0x");
  Serial.println(packet.command, HEX);
  
  // Update sensor data if needed
  if(packet.command == 0x02 || packet.command == 0x03) {
    // TRANSFER THE DATA TO PACKET!
    packet.data[0] = ltr.channel0;
    packet.data[1] = ltr.channel1;

    
    if(packet.command == 0x02) {
      Serial.println("Sending Channel 0");
    } else {
      Serial.println("Sending Channel 1");
    }
  }
  
  packet_send(&packet);
}