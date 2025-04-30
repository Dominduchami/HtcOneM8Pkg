ENTRY(_start);

SECTIONS
{
    _start = 0x11000000;
    . = 0x11000000;
    .data : {
        *(.data)
    }
}