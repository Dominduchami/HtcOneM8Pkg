ENTRY(_start);

SECTIONS
{
    _start = 0x10600000;
    . = 0x10600000;
    .data : {
        *(.data)
    }
}