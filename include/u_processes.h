#ifndef __USER_PROCESSES__
#define __USER_PROCESSES__

#include <s_processes.h>
/* Pid of various processes */
#define PROCESS_A1 7
#define PROCESS_B1 8
#define PROCESS_C1 9
#define PROCESS_A2 10 
#define PROCESS_B2 11 
#define PROCESS_C2 12 
#define PROCESS_A3 13 
/* Various commands for testing purposes */

#define DEBUG_U_PROCESSES

VOID process_a1(VOID);
VOID process_b1(VOID);
VOID process_c1(VOID);

VOID process_a2(VOID);
VOID process_b2(VOID);
VOID process_c2(VOID);

VOID process_a3(VOID);
/* Defines for messages types */
#define COUNT_REPORT 0x10
#define WAKEUP10 0x11


#endif
