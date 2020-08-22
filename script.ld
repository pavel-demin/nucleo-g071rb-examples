ENTRY(Reset_Handler)

_flash_start = ORIGIN(flash);

_stack_end = ORIGIN(ram) + LENGTH(ram);

MEMORY {
  flash (rx) : o = 0x08000000, l = 128k
  ram (wx) : o = 0x20000000, l = 32k
}

SECTIONS {
  .text : {
    . = ALIGN(4);
    _text_start = .;
    KEEP(* (.vector_table))
    * (.text*)
    * (.rodata*)
    * (.data*)
    . = ALIGN(4);
    _text_end = .;
  } >ram AT>flash

  .reset_handler : {
    . = ALIGN(4);
    * (.reset_handler)
    . = ALIGN(4);
  } >flash

  .bss : {
    . = ALIGN(4);
    _bss_start = .;
    * (.bss*)
    * (COMMON)
    . = ALIGN(4);
    _bss_end = .;
  } >ram
}
