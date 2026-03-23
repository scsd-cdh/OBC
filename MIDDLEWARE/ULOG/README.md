# uLog
uLog is a system heavily inspired from [defmt](https://defmt.ferrous-systems.com/) but implemented in C. The idea of
uLog is to save on CPU time and bandwidth by not performing the actual formatting on the embedded device but instead
at a later time on a separate machine.

## Usage
All formatting is done through the following macros which all work in the same way except for the log level:
- `ULOG_EMERGENCY()` / `ULOG_EMERG()`
- `ULOG_ALERT()` 
- `ULOG_CRITICAL()` / `ULOG_CRIT()` 
- `ULOG_ERROR()` / `ULOG_ERR()`
- `ULOG_WARNING()` / `ULOG_WARN()`
- `ULOG_NOTICE()`
- `ULOG_INFO()`
- `ULOG_DEBUG()` / `ULOG_DBG()`
- `ULOG_TRACE()`

The levels are loosely based on the [syslog severity levels](https://en.wikipedia.org/wiki/Syslog#Severity_level).

### Basic
The macros can be used to log messages.
```c++
ULOG_NOTICE("System initialization complete");
```

### Arguments
The macros can take arguments.
```c++
ULOG_ERR("Too many arguments, expected {}, got {}", 3, 4);

int value = 12;
ULOG_DEBUG("Value is {}", value);

char * name = "Alex";
ULOG_INFO("User {} logged in", name);

ULOG_INFO("float: {} double: {} string: {} bool: {} int: {}", 12.5f, 56.4, "String literal", true, 42);
```

### Slices
For debugging, entire structs can be logged via `ULOG_SLICE()` and `ULOG_SLICE_PTR()`.

`ULOG_SLICE()` can be used on a value to use it as a format argument.

`ULOG_SLICE_PTR()` can be used on a pointer to use what it points to as a format argument.
```c++
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
struct magic_struct * p_magic = &magic;

ULOG_TRACE("Debug values: {} {}", ULOG_SLICE(magic), ULOG_SLICE_PTR(p_magic));
```

### uLog Strings
Strings are normally serialized as their individual characters one by one. To save on bandwidth, strings can be
interned if all possible values are known in advance.

```c++
struct ulog_string mode1 = ULOG_STRING("Bing");
struct ulog_string mode2 = ULOG_STRING("Bong");

ULOG_WARN("Curren mode is: {}", rand() ? mode1 : mode2);
```

### Logging level
The default logging level is INFO; it can be queried and changed using `ulog_get_level()` and `ulog_set_level()`.
Entries of a log level higher than the selected one will be ignored and not sent to the sink.

```c++
ulog_set_level(ULOG_LEVEL_EMERGENCY);
ulog_set_level(ULOG_LEVEL_EMERG);
ulog_set_level(ULOG_LEVEL_ALERT);
ulog_set_level(ULOG_LEVEL_CRITICAL);
ulog_set_level(ULOG_LEVEL_CRIT);
ulog_set_level(ULOG_LEVEL_ERROR);
ulog_set_level(ULOG_LEVEL_ERR);
ulog_set_level(ULOG_LEVEL_WARNING);
ulog_set_level(ULOG_LEVEL_WARN);
ulog_set_level(ULOG_LEVEL_NOTICE);
ulog_set_level(ULOG_LEVEL_INFO);
ulog_set_level(ULOG_LEVEL_DEBUG);
ulog_set_level(ULOG_LEVEL_DBG);
ulog_set_level(ULOG_LEVEL_TRACE);
```

## Decoding
ulog_decoder at https://github.com/alexkar598/ulog-decoder can be used to decode the log entries. The program expects one or more paths to elf files with ulog entries and will take
ulog entries over stdin and report the formatted text over stdout.

## Integration
In order to use uLog in a project, the following must be done:
1) A system id must be defined using `ULOG_SYSTEM_ID()`.
    ```c++
    ULOG_SYSTEM_ID(0x1234) 
    ```
2) `ulog_external_acquire()` and `ulog_external_release()` have to be implemented.

    `ulog_external_acquire()` should attempt to acquire a lock and return true if the lock was acquired or false otherwise.

    `ulog_external_release()` should release that lock.
    ```c++
    static bool ulog_lock = false;
    bool ulog_external_acquire() {
        if (ulog_lock) return false;
        return ulog_lock = true;
    }
    
    void ulog_external_release() {
        ulog_lock = false;
    }
    ```
3) `ulog_external_write()` and `ulog_external_flush()` have to be implemented.

   `ulog_external_write()` should take a byte and write it out to some destination.

   `ulog_external_flush()` will be called once in a while to request that whatever buffer the entries are written to is flushed.
    ```c++
    void ulog_external_write(const uint8_t byte) {
        write(STDOUT_FILENO, &byte, 1);
    }

    void ulog_external_flush(void) {
        fflush(stdout);
    }
    ```