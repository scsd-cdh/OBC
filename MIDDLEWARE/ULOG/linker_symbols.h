#pragma once
#include <stdint.h>

// These definitions correspond to symbols in the ulog.ld linker script. They represent the starts and ends of various
// segments

extern uint8_t _sulog;
extern uint8_t _sulog_level;
extern uint8_t _sulog_level_emergency;
extern uint8_t _eulog_level_emergency;
extern uint8_t _sulog_level_alert;
extern uint8_t _eulog_level_alert;
extern uint8_t _sulog_level_critical;
extern uint8_t _eulog_level_critical;
extern uint8_t _sulog_level_error;
extern uint8_t _eulog_level_error;
extern uint8_t _sulog_level_warning;
extern uint8_t _eulog_level_warning;
extern uint8_t _sulog_level_notice;
extern uint8_t _eulog_level_notice;
extern uint8_t _sulog_level_info;
extern uint8_t _eulog_level_info;
extern uint8_t _sulog_level_debug;
extern uint8_t _eulog_level_debug;
extern uint8_t _sulog_level_trace;
extern uint8_t _eulog_level_trace;
extern uint8_t _eulog_level;
extern uint8_t _sulog_string;
extern uint8_t _eulog_string;
extern uint8_t _sulog_argument;
extern uint8_t _eulog_argument;
extern uint8_t _sulog_meta;
extern uint8_t _eulog_meta;
extern uint8_t _eulog;