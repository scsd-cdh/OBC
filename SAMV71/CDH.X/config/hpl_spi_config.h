/* Auto-generated config file hpl_spi_config.h */
#ifndef HPL_SPI_CONFIG_H
#define HPL_SPI_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

#include <peripheral_clk_config.h>

// Enable configuration of module
#ifndef CONF_SPI_0_ENABLE
#define CONF_SPI_0_ENABLE 1
#endif

// Set module in SPI Master mode
#ifndef CONF_SPI_0_MODE
#define CONF_SPI_0_MODE 0x01
#endif

// Set FIFO disable
#ifndef CONF_SPI_0_FIFO_DISABLE
#define CONF_SPI_0_FIFO_DISABLE 0x01
#endif

// Set peripheral select as fixed
#ifndef CONF_SPI_0_PS
#define CONF_SPI_0_PS 0x0
#endif

// Set chip select decode as directly
#ifndef CONF_SPI_0_PCSDEC
#define CONF_SPI_0_PCSDEC 0x0
#endif

// <h> Basic Configuration

// <o> Character Size
// <i> Bit size for all characters sent over the SPI bus (CHSIZE)
// <0x0=>8 bits
// <0x1=>9 bits
// <0x2=>10 bits
// <0x3=>11 bits
// <0x4=>12 bits
// <0x5=>13 bits
// <0x6=>14 bits
// <0x7=>15 bits
// <0x8=>16 bits
// <id> spi_master_character_size
#ifndef CONF_SPI_0_CHSIZE
#define CONF_SPI_0_CHSIZE 0x0
#endif

// <o> Baud rate <1-12000000>
// <i> The SPI data transfer rate. Note: (fspi_clock / baudrate) < 255
// <id> spi_master_baud_rate
#ifndef CONF_SPI_0_BAUD
#define CONF_SPI_0_BAUD 100000
#endif

// </h>

// <e> Advanced Configuration
// <id> spi_master_advanced
#ifndef CONF_SPI_0_ADVANCED
#define CONF_SPI_0_ADVANCED 0
#endif

// <o> Dummy byte <0x00-0xFFFF>
// <id> spi_master_dummybyte
// <i> Dummy byte used when reading data from the slave without sending any data
#ifndef CONF_SPI_0_DUMMYBYTE
#define CONF_SPI_0_DUMMYBYTE 0xffff
#endif

// <o> Clock Polarity
// <0=>The inactive state value of SPCK is logic level zero.
// <1=>The inactive state value of SPCK is logic level one.
// <i> Determines the inactive state value of the serial clock (SPCK).
// <id> spi_master_arch_cpol
#ifndef CONF_SPI_0_CPOL
#define CONF_SPI_0_CPOL 0x0
#endif

// <o> Clock Phase
// <0x0=>Data is changed on the leading edge of SPCK and captured on the following edge of SPCK.
// <0x1=>Data is captured on the leading edge of SPCK and changed on the following edge of SPCK.
// <i> Determines which edge of SPCK causes data to change and which edge causes data to be captured.
// <id> spi_master_arch_cpha
#ifndef CONF_SPI_0_NCPHA
#define CONF_SPI_0_NCPHA 0x1
#endif

// <o> Delay Before SPCK (ns) <0-255000>
// <i> This field defines the delay from NPCS falling edge (activation) to the first valid SPCK transition (in ns).
// <id> spi_master_dlybs
#ifndef CONF_SPI_0_DLY_SPCK
#define CONF_SPI_0_DLY_SPCK 1000
#endif

// <o> Delay Between Consecutive Transfers (ns) <0-8160000>
// <i> This field defines the delay between two consecutive transfers with the same peripheral without removing the chip select (in ns).
// <id> spi_master_dlybct
#ifndef CONF_SPI_0_DLY_BCT
#define CONF_SPI_0_DLY_BCT 1000
#endif

// </e>

/* Calculate baud register value from requested baudrate value */
#ifndef CONF_SPI_0_BAUD_RATE
#define CONF_SPI_0_BAUD_RATE (CONF_SPI0_FREQUENCY / CONF_SPI_0_BAUD)
#endif

/* Calculates the value of the CSR DLYBS field given the desired delay (in ns) */
#ifndef CONF_SPI_0_DLYBS
#define CONF_SPI_0_DLYBS (((CONF_SPI0_FREQUENCY / 1000000) * CONF_SPI_0_DLY_SPCK) / 1000)
#endif

/* Calculates the value of the CSR DLYBCT field given the desired delay (in ns) */
#ifndef CONF_SPI_0_DLYBCT
#define CONF_SPI_0_DLYBCT (((CONF_SPI0_FREQUENCY / 1000000) * CONF_SPI_0_DLY_BCT) / 32000)
#endif

// <<< end of configuration section >>>

#endif // HPL_SPI_CONFIG_H
