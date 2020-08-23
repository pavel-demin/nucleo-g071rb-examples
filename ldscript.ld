ENTRY(Reset_Handler)

_flash_start = ORIGIN(flash);
_sram_start = ORIGIN(sram);

_stack_end = ORIGIN(sram) + LENGTH(sram);

MEMORY {
  flash (rx) : o = 0x08000000, l = 128k
  sram (wx) : o = 0x20000000, l = 32k
}

SECTIONS {
  .text : {
    . = ALIGN(4);
    KEEP(* (.vector_table))
    * (.text*)
    . = ALIGN(4);
  } >sram AT>flash

  .rodata : {
    . = ALIGN(4);
    * (.rodata*)
    . = ALIGN(4);
  } >sram AT>flash

  .data : {
    . = ALIGN(4);
    * (.data*)
    . = ALIGN(4);
    _data_end = .;
  } >sram AT>flash

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
  } >sram
}
