
/**
 * \file
 *
 * \brief SAM Serial Peripheral Interface (SPI)
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#include <hpl_dma.h>
#include <hpl_spi_config.h>
#include <hpl_spi_m_async.h>
#include <hpl_spi_m_sync.h>
#include <hpl_spi_s_async.h>
#include <hpl_spi_s_sync.h>
#include <utils.h>
#include <utils_assert.h>

/** Build configuration from header macros. */
#define SPI_CONFIGURATION(n)                                                                                           \
	{                                                                                                                  \
		(n), (uint32_t)(CONF_SPI_##n##_FIFO_DISABLE << 31),                                                            \
		    ((CONF_SPI_##n##_MODE << 0) | (CONF_SPI_##n##_PS << 1) | (CONF_SPI_##n##_PCSDEC << 2)),                    \
		    ((CONF_SPI_##n##_CPOL << 0) | (CONF_SPI_##n##_NCPHA << 1) | (SPI_CSR_BITS(CONF_SPI_##n##_CHSIZE))          \
		     | (SPI_CSR_SCBR(CONF_SPI_##n##_BAUD_RATE)) | (SPI_CSR_DLYBS(CONF_SPI_##n##_DLYBS))                        \
		     | (SPI_CSR_DLYBCT(CONF_SPI_##n##_DLYBCT))),                                                               \
		    (CONF_SPI_##n##_DUMMYBYTE)                                                                                 \
	}

#ifndef CONF_SPI_0_ENABLE
#define CONF_SPI_0_ENABLE 0
#endif
#ifndef CONF_SPI_1_ENABLE
#define CONF_SPI_1_ENABLE 0
#endif

#ifndef CONF_SPI_0_M_DMA_TX_CHANNEL
#define CONF_SPI_0_M_DMA_TX_CHANNEL 0
#endif
#ifndef CONF_SPI_0_M_DMA_RX_CHANNEL
#define CONF_SPI_0_M_DMA_RX_CHANNEL 1
#endif

#ifndef CONF_SPI_1_M_DMA_TX_CHANNEL
#define CONF_SPI_1_M_DMA_TX_CHANNEL 0
#endif
#ifndef CONF_SPI_1_M_DMA_RX_CHANNEL
#define CONF_SPI_1_M_DMA_RX_CHANNEL 1
#endif

#ifndef CONF_SPI_0_RX_CHANNEL
#define CONF_SPI_0_RX_CHANNEL 0
#endif
#ifndef CONF_SPI_1_RX_CHANNEL
#define CONF_SPI_1_RX_CHANNEL 0
#endif

struct spi_regs_cfg {
	uint8_t           number;
	hri_spi_cr_reg_t  cr;
	hri_spi_mr_reg_t  mr;
	hri_spi_csr_reg_t csr;
	uint16_t          dummy_byte;
};

#define SPI_AMOUNT (CONF_SPI_0_ENABLE + CONF_SPI_1_ENABLE)

#if SPI_AMOUNT < 1
/** Dummy array for compiling. */
static const struct spi_regs_cfg spi_regs[1] = {{0}};
#else
static const struct spi_regs_cfg spi_regs[] = {
#if CONF_SPI_0_ENABLE
    SPI_CONFIGURATION(0),
#endif
#if CONF_SPI_1_ENABLE
    SPI_CONFIGURATION(1),
#endif
};
#endif

/**
 * \brief Retrieve IRQ number for the given hardware instance
 */
static IRQn_Type _spi_get_irq_num(const void *const hw)
{

	if (hw == SPI0) {
		return SPI0_IRQn;
	} else {
		return SPI1_IRQn;
	}
}

/**
 * \brief Init irq param with the given spi hardware instance
 */
static void _spi_init_irq_param(const void *const hw, struct _spi_async_dev *dev)
{
}

/** \internal De-initialize SPI
 *
 *  \param[in] hw Pointer to the hardware register base
 *
 * \return De-initialization status
 */
static int32_t _spi_deinit(void *const hw)
{
	hri_spi_write_CR_reg(hw, SPI_CR_SPIDIS | SPI_CR_SWRST);

	return ERR_NONE;
}

/** \internal Enable SPI
 *
 *  \param[in] hw Pointer to the hardware register base
 *
 * \return Enabling status
 */
static int32_t _spi_sync_enable(void *const hw)
{
	hri_spi_write_CR_reg(hw, SPI_CR_SPIEN);

	return ERR_NONE;
}

/** \internal Enable SPI
 *
 *  \param[in] hw Pointer to the hardware register base
 *
 * \return Enabling status
 */
static int32_t _spi_async_enable(void *const hw)
{
	_spi_sync_enable(hw);
	NVIC_EnableIRQ(_spi_get_irq_num(hw));

	return ERR_NONE;
}

/** \internal Disable SPI
 *
 *  \param[in] hw Pointer to the hardware register base
 *
 * \return Disabling status
 */
static int32_t _spi_sync_disable(void *const hw)
{
	hri_spi_write_CR_reg(hw, SPI_CR_SPIDIS);

	return ERR_NONE;
}

/** \internal Disable SPI
 *
 *  \param[in] hw Pointer to the hardware register base
 *
 * \return Disabling status
 */
static int32_t _spi_async_disable(void *const hw)
{
	_spi_sync_disable(hw);
	hri_spi_clear_IMR_reg(hw,
	                      SPI_IDR_RDRF | SPI_IDR_TDRE | SPI_IDR_MODF | SPI_IDR_OVRES | SPI_IDR_NSSR | SPI_IDR_TXEMPTY
	                          | SPI_IDR_UNDES);
	NVIC_DisableIRQ(_spi_get_irq_num(hw));

	return ERR_NONE;
}

/** \internal Set SPI mode
 *
 * \param[in] hw Pointer to the hardware register base
 * \param[in] mode The mode to set
 *
 * \return Setting mode status
 */
static int32_t _spi_set_mode(void *const hw, const enum spi_transfer_mode mode)
{
	uint32_t temp;

	temp = hri_spi_read_CSR_reg(hw, 0);
	temp &= ~(SPI_CSR_CPOL | SPI_CSR_NCPHA);

	switch (mode) {
	case SPI_MODE_0:
		temp |= (SPI_CSR_NCPHA_Msk);
		break;

	case SPI_MODE_1:
		/* CPOL = NCPHA = 0 already */
		break;

	case SPI_MODE_2:
		temp |= (SPI_CSR_NCPHA_Msk) | (SPI_CSR_CPOL_Msk);
		break;

	case SPI_MODE_3:
		temp |= (SPI_CSR_CPOL_Msk);
		break;

	default:
		break;
	}

	hri_spi_write_CSR_reg(hw, 0, temp);

	return ERR_NONE;
}

/** \internal Set SPI baudrate
 *
 * \param[in] hw Pointer to the hardware register base
 * \param[in] baud_val The baudrate to set
 *
 * \return Setting baudrate status
 */
static int32_t _spi_set_baudrate(void *const hw, const uint32_t baud_val)
{
	hri_spi_write_CSR_SCBR_bf(hw, 0, (uint8_t)baud_val);

	return ERR_NONE;
}

/** \internal Set SPI char size
 *
 * \param[in] hw       Pointer to the hardware register base
 * \param[in] baud_val The baudrate to set
 * \param[out] size    Stored char size
 *
 * \return Setting char size status
 */
static int32_t _spi_set_char_size(void *const hw, const enum spi_char_size char_size, uint8_t *const size)
{
	hri_spi_write_CSR_BITS_bf(hw, 0, char_size);
	if (char_size == SPI_CHAR_SIZE_8) {
		*size = 1;
	} else {
		*size = 2;
	}

	return ERR_NONE;
}

/**
 * \brief Retrieve ordinal number of the given SPI hardware instance
 */
static uint8_t _spi_get_hardware_index(const void *const hw)
{

	uint8_t index = ((uint32_t)hw - (uint32_t)SPI0) >> 16;

	if (index == 5) {
		return 1;
	} else {
		return 0;
	}
}

/** \brief Return the pointer to register settings of specific SPI
 *  \param[in] hw_addr The hardware register base address.
 *  \return Pointer to register settings of specific SERCOM.
 */
static inline const struct spi_regs_cfg *_spi_get_regs(const uint32_t hw_addr)
{
	uint8_t n = _spi_get_hardware_index((const void *)hw_addr);
	uint8_t i;

	for (i = 0; i < sizeof(spi_regs) / sizeof(struct spi_regs_cfg); i++) {
		if (spi_regs[i].number == n) {
			return &spi_regs[i];
		}
	}

	return NULL;
}

int32_t _spi_m_sync_init(struct _spi_m_sync_dev *dev, void *const hw)
{
	const struct spi_regs_cfg *regs = _spi_get_regs((uint32_t)hw);

	ASSERT(dev && hw);

	if (regs == NULL) {
		return ERR_INVALID_ARG;
	}

	if (hri_spi_get_SR_SPIENS_bit(hw)) {
		return ERR_DENIED;
	}
	hri_spi_write_CR_reg(hw, SPI_CR_SWRST);
	dev->prvt = hw;
	hri_spi_write_CR_reg(hw, (regs->cr & ~(SPI_CR_SPIEN | SPI_CR_SPIDIS | SPI_CR_SWRST | SPI_CR_LASTXFER)));
	hri_spi_write_MR_reg(hw, ((regs->mr | SPI_MR_PCS(0x0E) | SPI_MR_MODFDIS) & ~SPI_MR_LLB));
	hri_spi_write_CSR_reg(hw, 0, regs->csr);

	dev->dummy_byte = regs->dummy_byte;
	dev->char_size  = regs->csr & SPI_CSR_BITS_Msk;

	return ERR_NONE;
}

int32_t _spi_spi_m_async_init(struct _spi_m_async_dev *dev, void *const hw)
{
	struct _spi_m_async_dev *spid = dev;
	/* Do hardware initialize. */
	int32_t rc = _spi_m_sync_init((struct _spi_m_sync_dev *)dev, hw);

	if (rc < 0) {
		return rc;
	}

	/* Initialize callbacks: must use them */
	spid->callbacks.complete = NULL;
	spid->callbacks.rx       = NULL;
	spid->callbacks.tx       = NULL;

	_spi_init_irq_param(hw, dev);
	NVIC_DisableIRQ(_spi_get_irq_num(hw));
	NVIC_ClearPendingIRQ(_spi_get_irq_num(hw));

	return ERR_NONE;
}

int32_t _spi_m_sync_deinit(struct _spi_m_sync_dev *dev)
{
	void *hw = dev->prvt;

	ASSERT(dev && hw);

	return _spi_deinit(hw);
}

int32_t _spi_spi_m_async_deinit(struct _spi_m_async_dev *dev)
{
	NVIC_DisableIRQ(_spi_get_irq_num(dev->prvt));
	NVIC_ClearPendingIRQ(_spi_get_irq_num(dev->prvt));

	return _spi_deinit(dev->prvt);
}

int32_t _spi_m_sync_enable(struct _spi_m_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_sync_enable(dev->prvt);
}

int32_t _spi_spi_m_async_enable(struct _spi_m_async_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_async_enable(dev->prvt);
}

int32_t _spi_m_sync_disable(struct _spi_m_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_sync_disable(dev->prvt);
}

int32_t _spi_spi_m_async_disable(struct _spi_m_async_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_async_disable(dev->prvt);
}

int32_t _spi_m_sync_set_mode(struct _spi_m_sync_dev *dev, const enum spi_transfer_mode mode)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_mode(dev->prvt, mode);
}

int32_t _spi_spi_m_async_set_mode(struct _spi_m_async_dev *dev, const enum spi_transfer_mode mode)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_mode(dev->prvt, mode);
}

int32_t _spi_calc_baud_val(struct spi_dev *dev, const uint32_t clk, const uint32_t baud)
{
	int32_t rc;
	ASSERT(dev);

	/* Not accept 0es */
	if (clk == 0 || baud == 0) {
		return ERR_INVALID_ARG;
	}

	/* Check baudrate range of current assigned clock */
	if (!(baud <= (clk >> 1) && baud >= (clk >> 8))) {
		return ERR_INVALID_ARG;
	}

	rc = clk / baud;
	return rc;
}

int32_t _spi_m_sync_set_baudrate(struct _spi_m_sync_dev *dev, const uint32_t baud_val)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_baudrate(dev->prvt, baud_val);
}

int32_t _spi_spi_m_async_set_baudrate(struct _spi_m_async_dev *dev, const uint32_t baud_val)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_baudrate(dev->prvt, baud_val);
}

int32_t _spi_m_sync_set_data_order(struct _spi_m_sync_dev *dev, const enum spi_data_order dord)
{
	ASSERT(dev && dev->prvt);
	(void)dord;

	return ERR_UNSUPPORTED_OP;
}

int32_t _spi_spi_m_async_set_data_order(struct _spi_m_async_dev *dev, const enum spi_data_order dord)
{
	ASSERT(dev && dev->prvt);
	(void)dord;

	return ERR_UNSUPPORTED_OP;
}

int32_t _spi_m_sync_set_char_size(struct _spi_m_sync_dev *dev, const enum spi_char_size char_size)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_char_size(dev->prvt, char_size, &dev->char_size);
}

int32_t _spi_spi_m_async_set_char_size(struct _spi_m_async_dev *dev, const enum spi_char_size char_size)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_char_size(dev->prvt, char_size, &dev->char_size);
}

/** Wait until SPI bus idle. */
static inline void _spi_wait_bus_idle(void *const hw)
{
	while (!(hri_spi_get_SR_reg(hw, SPI_SR_TXEMPTY))) {
	}
}

/** Holds run time information for message sync transaction. */
struct _spi_trans_ctrl {
	/** Pointer to transmitting data buffer. */
	uint8_t *txbuf;
	/** Pointer to receiving data buffer. */
	uint8_t *rxbuf;
	/** Count number of data transmitted. */
	uint32_t txcnt;
	/** Count number of data received. */
	uint32_t rxcnt;
	/** Data character size. */
	uint8_t char_size;
};

/** Check flag of RXC and update transaction runtime information. */
static inline bool _spi_rx_check_and_receive(void *const hw, struct _spi_trans_ctrl *ctrl)
{
	uint32_t data;

	if (!(hri_spi_read_SR_reg(hw) & SPI_SR_RDRF)) {
		return false;
	}

	data = hri_spi_read_RDR_RD_bf(hw);

	if (ctrl->rxbuf) {
		*ctrl->rxbuf++ = (uint8_t)data;

		if (ctrl->char_size > 1) {
			*ctrl->rxbuf++ = (uint8_t)(data >> 8);
		}
	}

	ctrl->rxcnt++;

	return true;
}

/** Check flag of DRE and update transaction runtime information. */
static inline bool _spi_tx_check_and_send(void *const hw, struct _spi_trans_ctrl *ctrl, uint16_t dummy)
{
	uint32_t data;

	if (!(hri_spi_read_SR_reg(hw) & SPI_SR_TDRE)) {
		return false;
	}

	if (ctrl->txbuf) {
		data = *ctrl->txbuf++;

		if (ctrl->char_size > 1) {
			data |= (*ctrl->txbuf) << 8;
			ctrl->txbuf++;
		}
	} else {
		data = dummy;
	}

	ctrl->txcnt++;
	hri_spi_write_TDR_reg(hw, data);
	return true;
}

/** Check interrupt flag of ERROR and update transaction runtime information. */
static inline int32_t _spi_err_check(const uint32_t iflag, void *const hw)
{
	ASSERT(hw);

	if (SPI_SR_OVRES & iflag) {
		return ERR_OVERFLOW;
	}

	return ERR_NONE;
}

int32_t _spi_m_sync_trans(struct _spi_m_sync_dev *dev, const struct spi_msg *msg)
{
	void *                 hw   = dev->prvt;
	int32_t                rc   = 0;
	struct _spi_trans_ctrl ctrl = {msg->txbuf, msg->rxbuf, 0, 0, dev->char_size};

	ASSERT(dev && hw);

	/* SPI must be enabled to start synchronous transfer */
	if (!hri_spi_get_SR_SPIENS_bit(hw)) {
		return ERR_NOT_INITIALIZED;
	}

	for (;;) {
		if (!_spi_rx_check_and_receive(hw, &ctrl)) {
			/* In master mode, do not start next byte before previous byte received
			 * to make better output waveform */
			if (ctrl.rxcnt >= ctrl.txcnt) {
				_spi_tx_check_and_send(hw, &ctrl, dev->dummy_byte);
			}
		}
		rc = _spi_err_check(hri_spi_read_SR_reg(hw), hw);
		if (rc < 0) {
			break;
		}
		if (ctrl.txcnt >= msg->size && ctrl.rxcnt >= msg->size) {
			rc = ctrl.txcnt;
			break;
		}
	}

	/* Wait until SPI bus idle */
	_spi_wait_bus_idle(hw);

	return rc;
}

int32_t _spi_spi_m_async_enable_tx(struct _spi_m_async_dev *dev, bool state)
{
	void *hw = dev->prvt;

	ASSERT(dev && hw);

	if (state) {
		hri_spi_set_IMR_TDRE_bit(hw);
	} else {
		hri_spi_clear_IMR_TDRE_bit(hw);
	}

	return ERR_NONE;
}

int32_t _spi_spi_m_async_enable_rx(struct _spi_m_async_dev *dev, bool state)
{
	void *hw = dev->prvt;

	ASSERT(dev);
	ASSERT(hw);

	if (state) {
		hri_spi_set_IMR_RDRF_bit(hw);
	} else {
		hri_spi_clear_IMR_RDRF_bit(hw);
	}

	return ERR_NONE;
}

int32_t _spi_spi_m_async_enable_tx_complete(struct _spi_m_async_dev *dev, bool state)
{
	void *hw = dev->prvt;

	ASSERT(dev);
	ASSERT(hw);

	if (state) {
		hri_spi_set_IMR_TXEMPTY_bit(hw);
	} else {
		hri_spi_clear_IMR_TXEMPTY_bit(hw);
	}

	return ERR_NONE;
}

int32_t _spi_spi_m_async_write_one(struct _spi_m_async_dev *dev, uint16_t data)
{
	ASSERT(dev && dev->prvt);

	hri_spi_write_TDR_reg(dev->prvt, data);

	return ERR_NONE;
}

uint16_t _spi_spi_m_async_read_one(struct _spi_m_async_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return hri_spi_read_RDR_reg(dev->prvt);
}

int32_t _spi_spi_m_async_register_callback(struct _spi_m_async_dev *dev, const enum _spi_m_async_dev_cb_type cb_type,
                                           const FUNC_PTR func)
{
	typedef void (*func_t)(void);
	struct _spi_m_async_dev *spid = dev;

	ASSERT(dev && (cb_type < SPI_DEV_CB_N));

	func_t *p_ls  = (func_t *)&spid->callbacks;
	p_ls[cb_type] = (func_t)func;

	return ERR_NONE;
}

/**
 * \brief Enable/disable SPI master interrupt
 *
 * param[in] device The pointer to SPI master device instance
 * param[in] type The type of interrupt to disable/enable if applicable
 * param[in] state Enable or disable
 */
void _spi_spi_m_async_set_irq_state(struct _spi_m_async_dev *const device, const enum _spi_m_async_dev_cb_type type,
                                    const bool state)
{
	ASSERT(device);

	if (SPI_DEV_CB_ERROR == type) {
		if (state) {
			hri_spi_set_IMR_OVRES_bit(device->prvt);
		} else {
			hri_spi_clear_IMR_OVRES_bit(device->prvt);
		}
	}
}

/**
 * \brief Enable/disable SPI slave interrupt
 *
 * param[in] device The pointer to SPI slave device instance
 * param[in] type The type of interrupt to disable/enable if applicable
 * param[in] state Enable or disable
 */
void _spi_spi_s_async_set_irq_state(struct _spi_async_dev *const device, const enum _spi_async_dev_cb_type type,
                                    const bool state)
{
	_spi_spi_m_async_set_irq_state(device, type, state);
}

int32_t _spi_spi_s_sync_init(struct _spi_s_sync_dev *dev, void *const hw)
{
	return _spi_m_sync_init(dev, hw);
}

int32_t _spi_spi_s_async_init(struct _spi_s_async_dev *dev, void *const hw)
{
	struct _spi_s_async_dev *spid = dev;
	/* Do hardware initialize. */
	int32_t rc = _spi_spi_s_sync_init((struct _spi_s_sync_dev *)dev, hw);

	if (rc < 0) {
		return rc;
	}

	/* Initialize callbacks: must use them */
	spid->callbacks.complete = NULL;
	spid->callbacks.rx       = NULL;
	spid->callbacks.tx       = NULL;

	_spi_init_irq_param(hw, dev);
	NVIC_DisableIRQ(_spi_get_irq_num(hw));
	NVIC_ClearPendingIRQ(_spi_get_irq_num(hw));

	return ERR_NONE;
}

int32_t _spi_spi_s_sync_deinit(struct _spi_s_sync_dev *dev)
{
	void *hw = dev->prvt;

	ASSERT(dev && hw);

	return _spi_deinit(hw);
}

int32_t _spi_spi_s_async_deinit(struct _spi_s_async_dev *dev)
{
	NVIC_DisableIRQ(_spi_get_irq_num(dev->prvt));
	NVIC_ClearPendingIRQ(_spi_get_irq_num(dev->prvt));

	return _spi_deinit(dev->prvt);
}

int32_t _spi_spi_s_sync_enable(struct _spi_s_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_sync_enable(dev->prvt);
}

int32_t _spi_spi_s_async_enable(struct _spi_s_async_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_async_enable(dev->prvt);
}

int32_t _spi_spi_s_sync_disable(struct _spi_s_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_sync_disable(dev->prvt);
}

int32_t _spi_spi_s_async_disable(struct _spi_s_async_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_async_disable(dev->prvt);
}

int32_t _spi_spi_s_sync_set_mode(struct _spi_s_sync_dev *dev, const enum spi_transfer_mode mode)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_mode(dev->prvt, mode);
}

int32_t _spi_spi_s_async_set_mode(struct _spi_s_async_dev *dev, const enum spi_transfer_mode mode)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_mode(dev->prvt, mode);
}

int32_t _spi_spi_s_sync_set_data_order(struct _spi_s_sync_dev *dev, const enum spi_data_order dord)
{
	ASSERT(dev && dev->prvt);
	(void)dord;

	return ERR_UNSUPPORTED_OP;
}

int32_t _spi_spi_s_async_set_data_order(struct _spi_s_async_dev *dev, const enum spi_data_order dord)
{
	ASSERT(dev && dev->prvt);
	(void)dord;

	return ERR_UNSUPPORTED_OP;
}

int32_t _spi_spi_s_sync_set_char_size(struct _spi_s_sync_dev *dev, const enum spi_char_size char_size)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_char_size(dev->prvt, char_size, &dev->char_size);
}

int32_t _spi_spi_s_async_set_char_size(struct _spi_s_async_dev *dev, const enum spi_char_size char_size)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_char_size(dev->prvt, char_size, &dev->char_size);
}

uint16_t _spi_spi_s_sync_read_one(struct _spi_s_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return hri_spi_read_RDR_reg(dev->prvt);
}

int32_t _spi_spi_s_sync_write_one(struct _spi_s_sync_dev *dev, uint16_t data)
{
	ASSERT(dev && dev->prvt);

	hri_spi_write_TDR_reg(dev->prvt, data);

	return ERR_NONE;
}

bool _spi_spi_s_sync_is_tx_ready(struct _spi_s_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return hri_spi_get_SR_TDRE_bit(dev->prvt);
}

bool _spi_spi_s_sync_is_rx_ready(struct _spi_s_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return hri_spi_get_SR_RDRF_bit(dev->prvt);
}

bool _spi_spi_s_sync_is_ss_deactivated(struct _spi_s_sync_dev *dev)
{
	(void)dev;

	return false;
}

bool _spi_spi_s_sync_is_error(struct _spi_s_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return (hri_spi_read_SR_reg(dev->prvt)) & 0x04;
}

int32_t _spi_spi_s_async_enable_tx(struct _spi_s_async_dev *dev, bool state)
{
	void *hw = dev->prvt;

	ASSERT(dev && hw);

	if (state) {
		hri_spi_set_IMR_TDRE_bit(hw);
	} else {
		hri_spi_clear_IMR_TDRE_bit(hw);
	}

	return ERR_NONE;
}

int32_t _spi_spi_s_async_enable_rx(struct _spi_s_async_dev *dev, bool state)
{
	void *hw = dev->prvt;

	ASSERT(dev);
	ASSERT(hw);

	if (state) {
		hri_spi_set_IMR_RDRF_bit(hw);
	} else {
		hri_spi_clear_IMR_RDRF_bit(hw);
	}

	return ERR_NONE;
}

int32_t _spi_spi_s_async_enable_ss_detect(struct _spi_s_async_dev *dev, bool state)
{
	(void)dev;
	(void)state;
	return ERR_UNSUPPORTED_OP;
}

int32_t _spi_spi_s_async_write_one(struct _spi_s_async_dev *dev, uint16_t data)
{
	ASSERT(dev && dev->prvt);

	hri_spi_write_TDR_reg(dev->prvt, data);

	return ERR_NONE;
}

uint16_t _spi_spi_s_async_read_one(struct _spi_s_async_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return hri_spi_read_RDR_reg(dev->prvt);
}

int32_t _spi_spi_s_async_register_callback(struct _spi_s_async_dev *dev, const enum _spi_s_async_dev_cb_type cb_type,
                                           const FUNC_PTR func)
{
	return _spi_spi_m_async_register_callback(dev, cb_type, func);
}

/**
 * \brief Retrieve usart sync helper functions
 */
void *_spi_get_spi_m_sync(void)
{
	return (void *)NULL;
}

/**
 * \brief Retrieve usart async helper functions
 */
void *_spi_get_spi_m_async(void)
{
	return (void *)NULL;
}

/**
 * \brief Retrieve usart dma helper functions
 */
void *_spi_get_spi_m_dma(void)
{
	return (void *)NULL;
}

/**
 * \brief Retrieve usart sync helper functions
 */
void *_spi_get_spi_s_sync(void)
{
	return (void *)NULL;
}

/**
 * \brief Retrieve usart async helper functions
 */
void *_spi_get_spi_s_async(void)
{
	return (void *)NULL;
}
