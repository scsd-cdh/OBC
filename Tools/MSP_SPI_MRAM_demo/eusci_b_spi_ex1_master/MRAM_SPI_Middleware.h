void CS_LOW();
void CS_HIGH();
void initSPI();
void spiTransfer(uint8_t byte);
void readUniqueId(uint8_t id_buffer[4]);
