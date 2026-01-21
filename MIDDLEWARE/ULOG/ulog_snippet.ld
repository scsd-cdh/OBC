.ulog 0 (INFO) :
{
    _sulog = .;
    _sulog_level = .;
    . += 1;

    _sulog_level_emergency = .;
    KEEP(*(.ulog.level.emergency))
    _eulog_level_emergency = .;
    _sulog_level_alert = .;
    KEEP(*(.ulog.level.alert))
    _eulog_level_alert = .;
    _sulog_level_critical = .;
    KEEP(*(.ulog.level.critical))
    _eulog_level_critical = .;
    _sulog_level_error = .;
    KEEP(*(.ulog.level.error))
    _eulog_level_error = .;
    _sulog_level_warning = .;
    KEEP(*(.ulog.level.warning))
    _eulog_level_warning = .;
    _sulog_level_notice = .;
    KEEP(*(.ulog.level.notice))
    _eulog_level_notice = .;
    _sulog_level_info = .;
    KEEP(*(.ulog.level.info))
    _eulog_level_info = .;
    _sulog_level_debug = .;
    KEEP(*(.ulog.level.debug))
    _eulog_level_debug = .;
    _sulog_level_trace = .;
    KEEP(*(.ulog.level.trace))
    _eulog_level_trace = .;

    _eulog_level = .;

    _sulog_string = .;
    . += 1;
    KEEP(*(.ulog.string));
    _eulog_string = .;

    _sulog_argument = .;
    KEEP(*(.ulog.argument));
    _eulog_argument = .;

    _sulog_meta = .;
    KEEP(*(.ulog.meta));
    _eulog_meta = .;

    _eulog = .;
    ASSERT(_eulog_level < 65534, ".ulog.level section cannot contain more than 65534 interned strings");
    ASSERT((_eulog_string - _sulog_string) < 65534, ".ulog.string section cannot contain more than 65534 interned strings");
}