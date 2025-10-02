
#ifndef _VERSION_
#define _VERSION_

#include <stdint.h>

#define PDS_FW_VERSION  ( 1 )

#define HASH_SIZE       ( 8 )
static const uint8_t pds_fw_hash[HASH_SIZE] = {
    0xD, 0xE, 0xA, 0xD, 0xB, 0xE, 0xE, 0xF
};   

#endif // _VERSION_

