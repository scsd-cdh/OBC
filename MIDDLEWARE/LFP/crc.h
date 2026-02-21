#pragma once

#include <stddef.h>
#include <stdint.h>

#define LFP_CRC16_INIT (0xFFFF)

extern const uint16_t lfp_crc16_table[256];

static inline uint16_t lfp_crc16(const uint8_t *data, const size_t len) {
    uint16_t crc = LFP_CRC16_INIT;
    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ lfp_crc16_table[(crc ^ data[i]) & 0xff];
    }
    return crc;
}

static inline uint16_t lfp_crc16_update(const uint16_t crc, const uint8_t c) {
    return (crc >> 8) ^ lfp_crc16_table[(crc ^ c) & 0xff];
}


#define LFP_CRC32_INIT (0xFFFFFFFF)

extern const uint32_t lfp_crc32_table[256];

static inline uint32_t lfp_crc32(const uint8_t *data, const size_t len) {
    uint32_t crc = LFP_CRC32_INIT;
    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ lfp_crc32_table[(crc ^ data[i]) & 0xff];
    }
    return crc;
}

static inline uint32_t lfp_crc32_update(const uint32_t crc, const uint8_t c) {
    return (crc >> 8) ^ lfp_crc32_table[(crc ^ c) & 0xff];
}

