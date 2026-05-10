/*
* RM3100 test
*
* - Reads REVID (0x36) + HSHAKE (0x35)
* - Writes/reads CCX/CCY/CCZ (cycle count)
*
* Build (from repo root):
*   west build -p always -b sam_v71_xult/samv71q21b Tools/RM3100_Test_Zephyr
*/


#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <string.h>


/* ===== user knobs ===== */
#ifndef RM_ADDR_A
#define RM_ADDR_A 0x20u
#endif
#ifndef RM_ADDR_B
#define RM_ADDR_B 0x21u
#endif


#ifndef RM_CYCLE_COUNT
#define RM_CYCLE_COUNT 200u
#endif


#ifndef RM_TIMEOUT_MS
#define RM_TIMEOUT_MS 200u
#endif

#ifndef RM_DO_BIST
#define RM_DO_BIST 1
#endif


/* 0=repeated-start, 1=STOP between write and read */
#ifndef RM_STOP_BETWEEN
#define RM_STOP_BETWEEN 0
#endif


/* ===== RM3100 registers ===== */
#define RM_REG_POLL      0x00
#define RM_REG_CCX_MSB   0x04
#define RM_REG_CCY_MSB   0x06
#define RM_REG_CCZ_MSB   0x08
#define RM_REG_MX2       0x24
#define RM_REG_BIST      0x33
#define RM_REG_STATUS    0x34
#define RM_REG_HSHAKE    0x35
#define RM_REG_REVID     0x36


#define RM_POLL_XYZ      0x70u
#define RM_STATUS_DRDY   0x80u
#define RM_BIST_STE      0x80u


/*pick an I2C bus node*/
#if DT_NODE_HAS_STATUS(DT_ALIAS(i2c0), okay)
#define RM_I2C_NODE DT_ALIAS(i2c0)
#elif DT_NODE_HAS_STATUS(DT_NODELABEL(twihs0), okay)
#define RM_I2C_NODE DT_NODELABEL(twihs0)
#elif DT_NODE_HAS_STATUS(DT_NODELABEL(i2c0), okay)
#define RM_I2C_NODE DT_NODELABEL(i2c0)
#else
#error "No I2C node found (need DT alias i2c0 or nodelabel twihs0). Add an overlay to define it."
#endif


static const struct device *const i2c_dev = DEVICE_DT_GET(RM_I2C_NODE);


static int rm_read_reg(uint8_t addr7, uint8_t reg, uint8_t *out, size_t n)
{
#if RM_STOP_BETWEEN
    int r = i2c_write(i2c_dev, &reg, 1, addr7);
    if (r) return r;
    return i2c_read(i2c_dev, out, n, addr7);
#else
    return i2c_write_read(i2c_dev, addr7, &reg, 1, out, n);
#endif
}


static int rm_write_reg(uint8_t addr7, uint8_t reg, const uint8_t *data, size_t n)
{
    uint8_t tmp[1 + 16];
    if (n > 16) return -EINVAL;
    tmp[0] = reg;
    if (n) memcpy(&tmp[1], data, n);
    return i2c_write(i2c_dev, tmp, 1 + n, addr7);
}


static int rm_wait_drdy(uint8_t addr7)
{
    uint8_t st = 0;
    for (uint32_t t = 0; t < RM_TIMEOUT_MS; t++) {
        int r = rm_read_reg(addr7, RM_REG_STATUS, &st, 1);
        if (r) return r;
        if (st & RM_STATUS_DRDY) return 0;
        k_msleep(1);
    }
    return -ETIMEDOUT;
}


static int32_t s24_to_s32(uint8_t b2, uint8_t b1, uint8_t b0)
{
    uint32_t u = ((uint32_t)b2 << 16) | ((uint32_t)b1 << 8) | (uint32_t)b0;
    if (u & 0x00800000u) u |= 0xFF000000u;
    return (int32_t)u;
}


static int rm_test_one(uint8_t addr7)
{
    uint8_t revid = 0, hshake = 0;
    int r = 0;


    r = rm_read_reg(addr7, RM_REG_REVID, &revid, 1);
    if (r) return r;
    r = rm_read_reg(addr7, RM_REG_HSHAKE, &hshake, 1);
    if (r) return r;


    printk("\n[RM3100 0x%02X] REVID=0x%02X HSHAKE=0x%02X\n", addr7, revid, hshake);


    /* write CCX/CCY/CCZ starting at CCX_MSB (6 bytes, big-endian) */
    uint8_t cc[6] = {
        (uint8_t)(RM_CYCLE_COUNT >> 8), (uint8_t)(RM_CYCLE_COUNT & 0xFF),
        (uint8_t)(RM_CYCLE_COUNT >> 8), (uint8_t)(RM_CYCLE_COUNT & 0xFF),
        (uint8_t)(RM_CYCLE_COUNT >> 8), (uint8_t)(RM_CYCLE_COUNT & 0xFF),
    };
    r = rm_write_reg(addr7, RM_REG_CCX_MSB, cc, 6);
    if (r) return r;


    uint8_t rb[6] = {0};
    r = rm_read_reg(addr7, RM_REG_CCX_MSB, rb, 6);
    if (r) return r;


    uint16_t ccx = (uint16_t)(rb[0] << 8) | rb[1];
    uint16_t ccy = (uint16_t)(rb[2] << 8) | rb[3];
    uint16_t ccz = (uint16_t)(rb[4] << 8) | rb[5];
    printk("[RM3100 0x%02X] CCX=%u CCY=%u CCZ=%u\n", addr7, ccx, ccy, ccz);


    uint8_t poll = RM_POLL_XYZ;
#if RM_DO_BIST
    uint8_t bist = RM_BIST_STE;
    r = rm_write_reg(addr7, RM_REG_BIST, &bist, 1);
    if (r) return r;



    r = rm_write_reg(addr7, RM_REG_POLL, &poll, 1);
    if (r) return r;


    r = rm_wait_drdy(addr7);
    if (r) return r;


    r = rm_read_reg(addr7, RM_REG_BIST, &bist, 1);
    if (r) return r;


    printk("[RM3100 0x%02X] BIST raw=0x%02X\n", addr7, bist);
#endif


    /* single-shot measure */
    r = rm_write_reg(addr7, RM_REG_POLL, &poll, 1);
    if (r) return r;


    r = rm_wait_drdy(addr7);
    if (r) return r;


    uint8_t b[9] = {0};
    r = rm_read_reg(addr7, RM_REG_MX2, b, 9);
    if (r) return r;


    int32_t x = s24_to_s32(b[0], b[1], b[2]);
    int32_t y = s24_to_s32(b[3], b[4], b[5]);
    int32_t z = s24_to_s32(b[6], b[7], b[8]);


    printk("[RM3100 0x%02X] X=%ld Y=%ld Z=%ld (raw counts)\n", addr7, (long)x, (long)y, (long)z);


    return 0;
}


int main(void)
{
    printk("=== RM3100 test ===\n");
#if RM_STOP_BETWEEN
    printk("I2C mode: STOP between write/read\n");
#else
    printk("I2C mode: repeated-start\n");
#endif


    if (!device_is_ready(i2c_dev)) {
        printk("ERROR: I2C device not ready\n");
        return 0;
    }


    (void)i2c_configure(i2c_dev, I2C_MODE_CONTROLLER | I2C_SPEED_SET(I2C_SPEED_STANDARD));


    int r = rm_test_one(RM_ADDR_A);
    printk("Sensor A result: %d\n", r);


    r = rm_test_one(RM_ADDR_B);
    printk("Sensor B result: %d\n", r);


    printk("=== DONE ===\n");
    return 0;
}

