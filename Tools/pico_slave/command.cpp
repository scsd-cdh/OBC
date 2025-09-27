#include "command.h"

void packet_read(packet_t *packet, int count){
  if (count >= 2) {
    byte lowByte = Wire.read();
    byte highByte = Wire.read();
    packet->command = (highByte << 8) | lowByte;
    packet->newData = true;
  }
}

void packet_send(packet_t *packet){  
  // Always send exactly 4 bytes
  uint8_t response[4];
  
  // Ensure we have valid data to send
  response[0] = (packet->data[0] >> 8) & 0xFF;
  response[1] = packet->data[0] & 0xFF;
  response[2] = (packet->data[1] >> 8) & 0xFF;
  response[3] = packet->data[1] & 0xFF;
  
  // Send all bytes at once
  Wire.write(response, 4);
  
  // Debug
  Serial.print("Sent: ");
  Serial.print(packet->data[0]);
  Serial.print(", ");
  Serial.println(packet->data[1]);
}