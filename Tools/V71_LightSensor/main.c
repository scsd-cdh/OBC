#include <atmel_start.h>

// C Includes
#include <stdio.h>
#include <string.h>

/* Definition */
// Light Sensor LTR-303 or LTR-329
#define LTR_ALS_I2C_ADDRESS 0x29 // LTR ALS I2C address

#define LTR_ALS_CONTR 0x80 // ALS Control Register
#define LTR_ALS_MEASURE_RATE_ADDRESS 0x85 // ALS Measure Rate Register

#define LTR_ALS_DATA_CH1_LOW_ADDRESS 0x88 // ALS Data Channel 1 Low Byte Register
#define LTR_ALS_DATA_CH1_HIGH_ADDRESS 0x89 // ALS Data Channel 1 High Byte Register
#define LTR_ALS_DATA_CH0_LOW_ADDRESS 0x8A // ALS Data Channel 2 Low Byte Register
#define LTR_ALS_DATA_CH0_HIGH_ADDRESS 0x8B // ALS Data Channel 2 High Byte Register

/* Globals */
typedef struct {
	uint16_t als_data_ch1; // Channel 1 ALS data
	uint16_t als_data_ch0; // Channel 0 ALS data
	uint16_t als_infrared; // Infrared ALS data 
	uint16_t als_visible_light; // Visible light ALS data
} ltr_als_data_t;


/* Function Prototype */
void ltr_als_init(void);
void read_ltr_als_data(uint8_t lowAddr, uint8_t highAddr,uint16_t *data);
void print_ltr_als_data(ltr_als_data_t *ltr_als_data);

int main(void){
	struct io_descriptor *ltr_als_i2c_desc;
	ltr_als_data_t ltr_als_data;
	
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	// Initialize I2C for LTR ALS
	i2c_m_sync_get_io_descriptor(&I2C_0, &ltr_als_i2c_desc);
	i2c_m_sync_enable(&I2C_0);
	
	// Set I2C slave address
	i2c_m_sync_set_slaveaddr(&I2C_0, LTR_ALS_I2C_ADDRESS, I2C_M_SEVEN);

	// Initialize LTR ALS
	ltr_als_init();

	/* Replace with your application code */
	while (1) {
		// Read ALS data
		read_ltr_als_data(LTR_ALS_DATA_CH1_LOW_ADDRESS, LTR_ALS_DATA_CH1_HIGH_ADDRESS, &ltr_als_data.als_data_ch1);
		read_ltr_als_data(LTR_ALS_DATA_CH0_LOW_ADDRESS, LTR_ALS_DATA_CH0_HIGH_ADDRESS, &ltr_als_data.als_data_ch0);

		// Calculate Infrared and Visible Light data
		ltr_als_data.als_infrared = ltr_als_data.als_data_ch1;
		ltr_als_data.als_visible_light = ltr_als_data.als_data_ch0-ltr_als_data.als_data_ch1;

		// Print ALS data
		print_ltr_als_data(&ltr_als_data);

		// Delay for next measurement
		delay_ms(500); // 500ms delay for next measurement
	}
}

void ltr_als_init(void) {
	// Set values
	uint8_t control_value = 0x01; // 1x Gain with 200ms integration time
	uint8_t measure_rate_value = 0x12; // 200ms measurement rate 
	
	// Set ALS Control Register
	i2c_m_sync_cmd_write(&I2C_0,LTR_ALS_CONTR, &control_value, 1);

	// Set ALS Measure Rate Register
	i2c_m_sync_cmd_write(&I2C_0,LTR_ALS_MEASURE_RATE_ADDRESS, &measure_rate_value, 1);
}

void read_ltr_als_data(uint8_t lowAddr, uint8_t highAddr,uint16_t *data){
	uint8_t low_byte, high_byte;
	
	// Read Data from Channel
	i2c_m_sync_cmd_read(&I2C_0, lowAddr, &low_byte, 1); // low byte first
	i2c_m_sync_cmd_read(&I2C_0, highAddr, &high_byte, 1); // high byte second
	
	// Combine bytes into 16-bit data
	*data = (high_byte << 8) | low_byte;
}

void print_ltr_als_data(ltr_als_data_t *ltr_als_data){
	// Message Srings
	char msg_ch1[50] = "Channel 1 ALS Data: ";
	char msg_ch0[50] = "Channel 0 ALS Data: ";
	char msg_infrared[50] = "Infrared ALS Data: ";
	char msg_visible_light[50] = "Visible Light ALS Data: ";

	// Values
	uint16_t ch1_data = ltr_als_data->als_data_ch1;
	uint16_t ch0_data = ltr_als_data->als_data_ch0;
	uint16_t infrared_data = ltr_als_data->als_infrared;
	uint16_t visible_light_data = ltr_als_data->als_visible_light;

	// Store Data in Strings
	snprintf(msg_ch1, sizeof(msg_ch1), "%s%d", ch1_data);
	snprintf(msg_ch0, sizeof(msg_ch0), "%s%d", ch0_data);
	snprintf(msg_infrared, sizeof(msg_infrared), "%s%d", infrared_data);
	snprintf(msg_visible_light, sizeof(msg_visible_light), "%s%d", visible_light_data);

	// Print Data
	io_write(&I2C_0.io, (uint8_t *)msg_ch1, strlen(msg_ch1));
	io_write(&I2C_0.io, (uint8_t *)msg_ch0, strlen(msg_ch0));
	io_write(&I2C_0.io, (uint8_t *)msg_infrared, strlen(msg_infrared));
	io_write(&I2C_0.io, (uint8_t *)msg_visible_light, strlen(msg_visible_light));
}