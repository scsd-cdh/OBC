/* Auto-generated config file hpl_wdt_config.h */
#ifndef HPL_WDT_CONFIG_H
#define HPL_WDT_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Basic settings

// <o> Counter Value <1-4095>
// <i> Defines the value loaded in the 12-bit watchdog counter
// <id> wdt_counter_value
#ifndef CONF_WDT_WDV
#define CONF_WDT_WDV 4095
#endif

// <o> Delta Value <1-4095>
// <i> Defines the permitted range for reloading the Watchdog Timer
// <id> wdt_delta_value
#ifndef CONF_WDT_WDD
#define CONF_WDT_WDD 4095
#endif

// <q> Fault Interrupt Enable
// <i> A watchdog fault (underflow or error) asserts interrupt.
// <id> wdt_fault_interrupt_enable
#ifndef CONF_WDT_WDFIEN
#define CONF_WDT_WDFIEN 0
#endif

// <q> Reset Enable
// <i> A watchdog fault (underflow or error) triggers a watchdog reset.
// <id> wdt_fault_reset_enable
#ifndef CONF_WDT_WDRSTEN
#define CONF_WDT_WDRSTEN 1
#endif

// <q> WDT Disable
// <i> Determine if disable the wdt
// <id> wdt_disable
#ifndef CONF_WDT_WDDIS
#define CONF_WDT_WDDIS 0
#endif

// </h>

// <e> Advanced configuration
// <id> wdt_advanced
#ifndef WDT_ADVANCED_CONFIG
#define WDT_ADVANCED_CONFIG 0
#endif

// <q> Debug Halt
// <i> Determine if the watchdog halts when the processor is in debug state
// <id> wdt_debug_halt_enable
#ifndef CONF_WDT_WDDBGHLT
#define CONF_WDT_WDDBGHLT 1
#endif

// <q> IDLE Halt
// <i> Determine if the watchdog halts when the processor is in idle state
// <id> wdt_idle_halt_enable
#ifndef CONF_WDT_WDIDLEHLT
#define CONF_WDT_WDIDLEHLT 1
#endif

// </e>

// <<< end of configuration section >>>

#endif // HPL_WDT_CONFIG_H
