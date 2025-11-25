#include "logging.h"

#include <hal_usart_sync.h>
#include <CDH/driver_init.h>
#include <ulog/ulog.h>


ULOG_SYSTEM_ID(0x1234)

static struct io_descriptor *p_io = nullptr;

void init_logging(void)
{
#ifdef CDH_EVALBOARD
    usart_sync_get_io_descriptor(&ULOG_USART_1, &p_io);
    usart_sync_enable(&ULOG_USART_1);
#endif
}

static bool b_ulog_locked = false;
bool ulog_external_acquire(void)
{
    if (b_ulog_locked) return false;
    return b_ulog_locked = true;
}
void ulog_external_release(void)
{
    b_ulog_locked = false;
}
void ulog_external_write(const uint8_t byte)
{
#ifdef CDH_EVALBOARD
    io_write(p_io, &byte, 1);
#endif
}
void ulog_external_flush(void)
{
}