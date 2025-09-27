#ifndef LTR_H
#define LTR_H

/* Includes */
#include <Wire.h>

/* Defnitions */
// LTR I2C Address
#define LTR_ADDRESS 0X29

// LTR REGISTERS
#define LTR_ALS_CONTR_ADDRESS 0x80
#define LTR_ALS_MEAS_RATE_ADDRESS 0x85

/* Constants */
const uint8_t ch1[2] = {0x88, 0x89};
const uint8_t ch0[2] = {0x8A, 0x8B};

/* Typedef Structs */
typedef struct ltr_t {
  uint8_t gain, measureRate;
  uint16_t channel0, channel1;
} ltr_t;


/* Prototypes */
void ltr_init(ltr_t *ltr);
void ltr_read_channel(ltr_t *ltr, const uint8_t *channel);
bool ltr_test_connection();
uint8_t ltr_read_part_id();

#endif