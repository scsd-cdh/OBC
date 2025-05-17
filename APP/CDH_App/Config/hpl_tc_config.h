/* Auto-generated config file hpl_tc_config.h */
#ifndef HPL_TC_CONFIG_H
#define HPL_TC_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef CONF_TC0_ENABLE
#define CONF_TC0_ENABLE 1
#endif

#include "peripheral_clk_config.h"

// <h> Basic configuration

// <e> Divided Clock
// <id> timer_div_clock
#ifndef CONF_TC0_DIVIDE_CLOCK_ENABLE
#define CONF_TC0_DIVIDE_CLOCK_ENABLE 1
#endif

// <o> Clock Selection
// <0x0=> TIMER_CLOCK1 (PCK6)
// <0x1=> TIMER_CLOCK2 (MCK/8)
// <0x2=> TIMER_CLOCK3 (MCK/32)
// <0x3=> TIMER_CLOCK4 (MCK/128)
// <0x4=> TIMER_CLOCK5 (SLCK)
// <i> This defines the clock selection
// <id> timer_prescaler
#ifndef CONF_TC0_TCCLKS
#define CONF_TC0_TCCLKS 0x1
#endif

// </e>

// <o> Length of one timer tick in uS <0-4294967295>
// <id> timer_tick
#ifndef CONF_TC0_TIMER_RC_TICK
#define CONF_TC0_TIMER_RC_TICK 1000
#endif
// </h>

// <h> Event Control
// <e> Event Output Enable
// <id> timer_event_output_wave
#ifndef CONF_TC0_EVENT_OUTPUT_WAVE
#define CONF_TC0_EVENT_OUTPUT_WAVE 0
#endif

// <o> Length of one timer RA tick in uS <0-4294967295>
// <id> timer_ra_tick
#ifndef CONF_TC0_TIMER_RA_TICK
#define CONF_TC0_TIMER_RA_TICK 500
#endif
//</e>

// <e> External Event/Trigger Control in Capture Mode
// <id> timer_external_trigger_enable
#ifndef CONF_TC0_CPCTRG
#define CONF_TC0_CPCTRG 0
#endif

// <o> External Trigger Edge Selection
// <0x0=> The clock is not gated by an external signal
// <0x1=> Rising edge
// <0x2=> Falling edge
// <0x3=> Each edge
// <i> This defines external trigger edge selection.
//<id> timer_etrgedg
#ifndef CONF_TC0_ETRGEDG
#define CONF_TC0_ETRGEDG 0
#endif

// <o> TIOA or TIOB External Trigger Selection
// <0x0=> TIOB is used as an external trigger
// <0x1=> TIOA is used as an external trigger
// <i> This defines whether external trigger selection is TIOB or TIOA.
//<id> timer_abetrg
#ifndef CONF_TC0_ABETRG
#define CONF_TC0_ABETRG 0
#endif

// </e>

// <e> Fault Output Control
// <id> timer_fault_control
#ifndef CONF_TC0_FAULT_CONTROL
#define CONF_TC0_FAULT_CONTROL 0
#endif

// <q> Compare Fault Channel0 Enable
// <i> This defines whether the FAULT output source (CPCS flag) is enable or disable from channel0.
// <id> timer_fault_chan0_enable
#ifndef CONF_TC0_FAULT_CHAN0_ENABLE
#define CONF_TC0_FAULT_CHAN0_ENABLE 0
#endif

// <q> Compare Fault Channel1 Enable
// <i> This defines whether the FAULT output source (CPCS flag) is enable or disable from channel1.
// <id> timer_fault_chan1_enable
#ifndef CONF_TC0_FAULT_CHAN1_ENABLE
#define CONF_TC0_FAULT_CHAN1_ENABLE 0
#endif
// </e>

// </h>

// Default values which the driver needs in order to work correctly
// RB register set to 0
#ifndef CONF_TC0_RB
#define CONF_TC0_RB 0
#endif

// Clock set not invert
#ifndef CONF_TC0_CLKI
#define CONF_TC0_CLKI 0
#endif

// The clock is not gated by an external signal.
#ifndef CONF_TC0_BURST
#define CONF_TC0_BURST TC_CMR_BURST_NONE
#endif

// No Divided Clock or not
#define CONF_TC0_NODIVCLK 0
#if CONF_TC0_DIVIDE_CLOCK_ENABLE == 0
#undef CONF_TC0_NODIVCLK
#define CONF_TC0_NODIVCLK 1
#endif

// Calculating correct top value based on requested tick interval.
#if CONF_TC0_TCCLKS == 0x0
#define CONF_TC0_CLKS_FREQUENCY CONF_PCK6_FREQUENCY

#elif CONF_TC0_TCCLKS == 0x1
#define CONF_TC0_CLKS_FREQUENCY CONF_MCK_FREQUENCY / 8

#elif CONF_TC0_TCCLKS == 0x2
#define CONF_TC0_CLKS_FREQUENCY CONF_MCK_FREQUENCY / 32

#elif CONF_TC0_TCCLKS == 0x3
#define CONF_TC0_CLKS_FREQUENCY CONF_MCK_FREQUENCY / 128

#elif CONF_TC0_TCCLKS == 0x4
#define CONF_TC0_CLKS_FREQUENCY CONF_SLCK_FREQUENCY
#endif

#if CONF_TC0_DIVIDE_CLOCK_ENABLE == 0
#undef CONF_TC0_CLKS_FREQUENCY
#define CONF_TC0_CLKS_FREQUENCY CONF_TC0_FREQUENCY
#endif

#ifndef CONF_TC0_RA
#define CONF_TC0_RA (uint32_t)((float)CONF_TC0_TIMER_RA_TICK * CONF_TC0_CLKS_FREQUENCY / 1000000.f)
#endif

#ifndef CONF_TC0_RC
#define CONF_TC0_RC (uint32_t)((float)CONF_TC0_TIMER_RC_TICK * CONF_TC0_CLKS_FREQUENCY / 1000000.f)
#endif

#ifndef CONF_TC0_CMR_REG_VAL
#define CONF_TC0_CMR_REG_VAL                                                                                           \
	(TC_CMR_TCCLKS(CONF_TC0_TCCLKS) | CONF_TC0_CLKI | TC_CMR_BURST(CONF_TC0_BURST)                                     \
	 | TC_CMR_CAPTURE_ETRGEDG(CONF_TC0_ETRGEDG) | (CONF_TC0_ABETRG << TC_CMR_CAPTURE_ABETRG_Pos)                       \
	 | (((~CONF_TC0_EVENT_OUTPUT_WAVE) & CONF_TC0_CPCTRG) << 14) | ((CONF_TC0_EVENT_OUTPUT_WAVE << 15) & TC_CMR_WAVE)  \
	 | (2 << 13) /* WAVSEL = 10 */                                                                                     \
	 | TC_CMR_WAVE)
#endif

#ifndef CONF_TC0_TICKS_PER_US
#define CONF_TC0_TICKS_PER_US 0
#endif

// <<< end of configuration section >>>

#endif // HPL_TC_CONFIG_H
