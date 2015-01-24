#ifndef __STDIO_H__
#define __STDIO_H__

SINT32 rtx_dbug_outs (CHAR * s);
SINT32 rtx_dbug_number (UINT8 * number);

SINT32 rtx_print_integer (UINT32 integer);
SINT32 rtx_print_hex (SINT32 integer);

VOID uart_print_string(char *);
VOID uart_print_integer(int );

#endif
