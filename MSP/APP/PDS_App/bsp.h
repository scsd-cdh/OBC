
//*******************************************************************************
// Device Configuration, Pinout and defines *************************************
//*******************************************************************************

#ifndef _BSP_
#define _BSP_

#define SLAVE_ADDR                  ( 0x08 )  /**< I2C Slave Address for the MSP430 device */

#define CONV_RUN_A_PORT             ( GPIO_PORT_P1 )
#define CONV_RUN_A_PIN              ( GPIO_PIN5 )
#define CONV_RUN_B_PORT             ( GPIO_PORT_P2 )
#define CONV_RUN_B_PIN              ( GPIO_PIN4 )


#define CONV_FLAG1_X_PLUS_PORT      ( GPIO_PORT_P3 )
#define CONV_FLAG1_X_PLUS_PIN       ( GPIO_PIN7 )

#define CONV_FLAG2_X_PLUS_PORT      ( GPIO_PORT_P2 )
#define CONV_FLAG2_X_PLUS_PIN       ( GPIO_PIN3 )

#define CONV_FLAG1_X_MINUS_PORT     ( GPIO_PORT_P2 )
#define CONV_FLAG1_X_MINUS_PIN      ( GPIO_PIN2 )

#define CONV_FLAG2_X_MINUS_PORT     ( GPIO_PORT_P2 )
#define CONV_FLAG2_X_MINUS_PIN      ( GPIO_PIN1 )

#define CONV_FLAG1_Y_PLUS_PORT      ( GPIO_PORT_P2 )
#define CONV_FLAG1_Y_PLUS_PIN       ( GPIO_PIN0 )

#define CONV_FLAG2_Y_PLUS_PORT      ( GPIO_PORT_P4 )
#define CONV_FLAG2_Y_PLUS_PIN       ( GPIO_PIN1 )

#define CONV_FLAG1_Y_MINUS_PORT     ( GPIO_PORT_P4 )
#define CONV_FLAG1_Y_MINUS_PIN      ( GPIO_PIN0 )

#define CONV_FLAG2_Y_MINUS_PORT     ( GPIO_PORT_P4 )
#define CONV_FLAG2_Y_MINUS_PIN      ( GPIO_PIN2 )

#define INT_5V_VS                   ( P4_2 )    // TODO update configure
#define REG_5V_VS                   ( P1_0 )    // TODO update configure
#define A_5V_VS                     ( P1_1 )    // TODO update configure
#define B_5V_VS                     ( P1_2 )    // TODO update configure
#define TEMP_SENSE                  ( P1_3 )    // TODO update configure

#define VOLT_ADCs_THRESHOLD         (1000)  // TODO configure

void initBSP();

#endif // _BSP_
