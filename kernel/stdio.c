#include <rtx.h>
#include <rtx_inc.h>
#include <string.h>

/*
 * C Function wrapper for TRAP #15 function to output a character
 */


VOID
rtx_dbug_out_char (CHAR c)
{

  /* Store registers */
  asm ("move.l %d0, -(%a7)");
  asm ("move.l %d1, -(%a7)");

  /* Load CHAR c into d1 */
  asm ("move.l 8(%a6), %d1");

  /* Setup trap function */
  asm ("move.l #0x13, %d0");
  asm ("trap #15");

  /* Restore registers  */
  asm (" move.l %d1, (%a7)+");
  asm (" move.l %d0, (%a7)+");

}

/*
 * Print a C-style null terminated string
 */
SINT32
rtx_dbug_outs (CHAR * s)
{
  if (s == NULL)
    {
      return RTX_ERROR;
    }
  while (*s != '\0')
    {
      rtx_dbug_out_char (*s++);
    }
  return RTX_SUCCESS;
}


SINT32
rtx_dbug_number (UINT32 number)
{
  char str[2];
  str[0] = number + 0x30;
  str[1] = '\0';
  return (rtx_dbug_outs (str));
}

SINT32
rtx_print_integer (UINT32 integer)
{
  rtx_dbug_outs (IntToStr (integer));
  return 0; 
}
