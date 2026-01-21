#pragma once

/**
 * Initialize the logging system
 */
void logging_init(void);

/**
 * Force ulog to reset. This may corrupt the current frame and potentially the next one.
 */
void logging_force_reset(void);