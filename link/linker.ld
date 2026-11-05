ENTRY(_start)

PHDRS
{
  text PT_LOAD FLAGS(5); /* R=4 + X=1 */
  data PT_LOAD FLAGS(6); /* R=4 + W=2 */
}

SECTIONS
{
  . = 1M;

  .multiboot ALIGN(8):
  {
    *(.multiboot)
  } :text

  .text :
  {
    *(.text)
  } :text

  .rodata :
  {
    *(.rodata)
  } :text

  . = ALIGN(0x1000);

  .data :
  {
    *(.data)
  } :data

  .bss ALIGN(16):
  {
    *(.bss)
  } :data
}
