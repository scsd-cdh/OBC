#ifndef COMMAND_H
#define COMMAND_H

/* Include */
#include <Wire.h>
#include <string.h>

/* Definitions */
#define DEVICE_ADDRESS 0x42

/* Typedef */
typedef struct packet_t{
  bool newData;
  uint8_t command; // Command Received from Master
  uint16_t data[2]; // Data to send to master  
}packet_t;

/* Global */
void packet_read(packet_t *packet, int count);
void packet_send(packet_t *packet);


#endif
