#pragma once
#include <stdint.h>

struct ulog_rzcobs_state {
    uint8_t run;
    uint8_t zeros;
    void (*write)(uint8_t);
};

void ulog_rzcobs_encoder_write(struct ulog_rzcobs_state * self, uint8_t byte);
void ulog_rzcobs_encoder_end(struct ulog_rzcobs_state * self);