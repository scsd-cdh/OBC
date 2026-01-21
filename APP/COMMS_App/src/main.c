#include "main.h"

#include <stdlib.h>
#include <ulog/ulog.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

ULOG_SYSTEM_ID(0x3399)

int main(void) {
    int test = 5;

    // ReSharper disable once CppDFAEndlessLoop
    while (true) {
        printk("meow!");
        ULOG_INFO("Hi! (30 / test) is: {}", 30 / (test--));

        k_msleep(1000);
    }
}
