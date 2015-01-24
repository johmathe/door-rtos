#ifndef __S_PROCESSES__
#define __S_PROCESSES__

#define MAX_COMMAND_SIZE 32
#define CRT_PID 0x20 
#define KCD_PID 0x21
#define WC_PID 0x22
#define SET_PRIO_PID 0x23
#define NULL_PID 0

UINT32 register_command(CHAR c);
VOID kcd_process (VOID);
VOID wall_clock_process(VOID);
VOID crt_process(VOID);
VOID set_prio_process(VOID);
VOID null_process(VOID);

#define COMMAND_REGISTRATION 0x20
#define KEYBOARD_INPUT 0x21
#define CRT_DISPLAY_REQUEST 0x22
#define CRT_DISPLAY_NO_PROMPT 0x23

#endif
