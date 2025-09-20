// Shows how to use SPISlave on a single device.
// Core1 runs the SPI Slave mode and provides a unique reply to messages from the master
// Released to the public domain 2023 by Earle F. Philhower, III <earlephilhower@yahoo.com>

#include <SPI.h>
#include <SPISlave.h>

SPISettings spisettings(1000000, MSBFIRST, SPI_MODE0);
volatile bool recvBuffReady = false;
char recvBuff[42] = "";
int recvIdx = 0;
void recvCallback(uint8_t *data, size_t len) {
  memcpy(recvBuff + recvIdx, data, len);
  recvIdx += len;
  if (recvIdx == sizeof(recvBuff)) {
    recvBuffReady = true;
    recvIdx = 0;
  }
}

int sendcbs = 0;
// Note that the buffer needs to be long lived, the SPISlave doesn't copy it.  So no local stack variables, only globals or heap(malloc/new) allocations.
char sendBuff[42];
void sentCallback() {
  memset(sendBuff, 0, sizeof(sendBuff));
  sprintf(sendBuff, "Slave to Master Xmission %d", sendcbs++);
  SPISlave.setData((uint8_t*)sendBuff, sizeof(sendBuff));
}

// Note that we use SPISlave1 here **not** because we're running on
// Core 1, but because SPI0 is being used already.  You can use
// SPISlave or SPISlave1 on any core.
void setup() {
  SPISlave.setRX(8);
  SPISlave.setCS(9);
  SPISlave.setSCK(10);
  SPISlave.setTX(11);
  // Ensure we start with something to send...
  sentCallback();
  // Hook our callbacks into the slave
  SPISlave.onDataRecv(recvCallback);
  SPISlave.onDataSent(sentCallback);
  SPISlave.begin(spisettings);
  delay(3000);
  Serial.println("S-INFO: SPISlave started");
}

int count = 0;
void loop() {
  if (count++ % 1000 == 0) {
    Serial.println("Looping");
  }
  if (recvBuffReady) {
    Serial.printf("S-RECV: '%s'\n", recvBuff);
    recvBuffReady = false;
  }
}