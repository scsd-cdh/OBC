#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <ulog/ulog.h>

void ulog_external_write(const uint8_t byte) {
    write(STDOUT_FILENO, &byte, 1);
}

void ulog_external_flush(void) {
    fflush(stdout);
}

static bool ulog_lock = false;
bool ulog_external_acquire() {
    if (ulog_lock) return false;
    return ulog_lock = true;
}

void ulog_external_release() {
    ulog_lock = false;
}

int main(int argc, char *argv[]) {
    const struct ulog_string my_string = ULOG_STRING("magical and based");

    struct magic_struct {
        char first[8];
        int bullshit;
        float whoa;
        char name[64] ;
    };
    struct magic_struct magic = {
        .first = "12345678",
        .bullshit = 123,
        .whoa = 6.9f,
        .name = "meow meow"
    };
    struct magic_struct* pMagic = &magic;

    struct ulog_slice test2 = ULOG_SLICE(magic);

    const uint32_t tes = 12;
    bool my_bool = true;

    const char * test = "pointer";
    ULOG_ALERT("whowsa! I have naugty chars such as \" and \n also \\ :3");
    ULOG_ALERT("many many arguments {} {}", test, ULOG_SLICE(magic));
    constexpr char array[] = "array";
    ulog_set_level(ULOG_LEVEL_INFO);
    ULOG_ALERT("ptr test {} {} {} {} {} {} {}", "literal", test, array, 0x987654321, ULOG_SLICE(magic), "~~~~~~~", ULOG_SLICE_PTR(pMagic));
    ULOG_INFO("test");
    ULOG_ALERT("array {}", array);
    ULOG_ALERT("user string");

    ULOG_WARNING("user string {} {} {}", my_string, my_string, my_string);
    ULOG_ALERT("user string {}", my_bool ? ULOG_STR("abc") : ULOG_STR("cde"));

    ULOG_TRACE("Should be ignored");
    ulog_set_level(ULOG_LEVEL_TRACE);
    ULOG_EMERG("0");
    ULOG_ALERT("1");
    ULOG_CRIT("2");
    ULOG_ERROR("3");
    ULOG_WARN("4");
    ULOG_NOTICE("5");
    ULOG_INFO("6");
    ULOG_DEBUG("7");
    ULOG_TRACE("8");

    ULOG_NOTICE("Something has happened, please remember to stay hydrated at all times! {} {} {} {} {} {}", "MEOWSERS!", test, 12, 15u, 22.0/7, true);

    ULOG_INFO("Format {} string {} test", 1, 2);

    return 0;
}

ULOG_SYSTEM_ID(0x1234)
