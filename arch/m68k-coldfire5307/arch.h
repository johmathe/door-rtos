#define SEND_MESSAGE 1
#define RECEIVE_MESSAGE 2 
#define REQUEST_MEMORY 3
#define RELEASE_MEMORY 4
#define RELEASE_PROCESSOR 5
#define DELAYED_SEND 6
#define SET_PRIO 7
#define GET_PRIO 8 


#define SET_SP( )  asm("move.l tmp_sp , %sp")

#define GET_SP( )  asm("move.l %sp , tmp_sp " ) 

/*
 * Store the timer ISR at auto-vector #6
 */

#define INIT_TIMER_INTERRUPT() \
    asm( "move.l %d0, -(%a7)" ); \
    asm( "move.l #timer_handler,%d0" ); \
    asm( "move.l %d0,0x10000078" ); \
    asm( "move.l (%a7)+, %d0" ); 

/*
 * Store the UART ISR at auto-vector #64
 */


#define INIT_UART_INTERRUPT() \
    asm( "move.l %d0, -(%a7)" ); \
    asm( "move.l #uart_handler,%d0" ); \
    asm( "move.l %d0,0x10000100" ); \
    asm( "move.l (%a7)+, %d0" );  



#define DISABLE_INTERRUPTS() \
    asm( "move.w #0x2700,%sr" );

#define ENABLE_INTERRUPTS() \
    asm( "move.w #0x2000,%sr" );

#define INIT_TRAP_HANDLER() \
asm("move.l #swi_trap_handler,%d0 \n\t"); \
asm("move.l %d0,0x10000080");

#define SAVE_CONTEXT() \
    asm("move.l %d0, -(%sp)");\
    asm("move.l %d1, -(%sp)");\
    asm("move.l %d2, -(%sp)");\
    asm("move.l %d3, -(%sp)");\
    asm("move.l %d4, -(%sp)");\
    asm("move.l %d5, -(%sp)");\
    asm("move.l %d6, -(%sp)");\
    asm("move.l %d7, -(%sp)");\
    asm("move.l %a0, -(%sp)");\
    asm("move.l %a1, -(%sp)");\
    asm("move.l %a2, -(%sp)");\
    asm("move.l %a3, -(%sp)");\
    asm("move.l %a4, -(%sp)");\
    asm("move.l %a5, -(%sp)");\
    asm("move.l %a6, -(%sp)");


/* Here, we assume gcc uses the %a6 register for its fp*/
#define UNLK_FP() \
    asm("unlk %a6");

#define TRAP0() \
   asm("trap #0");

#define RETURN_FROM_EXCEPTION() \
    asm("rte");

#define RESTORE_CONTEXT() \
    asm("move.l (%sp)+, %a6");\
    asm("move.l (%sp)+, %a5");\
    asm("move.l (%sp)+, %a4");\
    asm("move.l (%sp)+, %a3");\
    asm("move.l (%sp)+, %a2");\
    asm("move.l (%sp)+, %a1");\
    asm("move.l (%sp)+, %a0");\
    asm("move.l (%sp)+, %d7");\
    asm("move.l (%sp)+, %d6");\
    asm("move.l (%sp)+, %d5");\
    asm("move.l (%sp)+, %d4");\
    asm("move.l (%sp)+, %d3");\
    asm("move.l (%sp)+, %d2");\
    asm("move.l (%sp)+, %d1");\
    asm("move.l (%sp)+, %d0");


#define RESTORE_CONTEXT_EXCEPT_D0() \
    asm("move.l (%sp)+, %a6");\
    asm("move.l (%sp)+, %a5");\
    asm("move.l (%sp)+, %a4");\
    asm("move.l (%sp)+, %a3");\
    asm("move.l (%sp)+, %a2");\
    asm("move.l (%sp)+, %a1");\
    asm("move.l (%sp)+, %a0");\
    asm("move.l (%sp)+, %d7");\
    asm("move.l (%sp)+, %d6");\
    asm("move.l (%sp)+, %d5");\
    asm("move.l (%sp)+, %d4");\
    asm("move.l (%sp)+, %d3");\
    asm("move.l (%sp)+, %d2");\
    asm("move.l (%sp)+, %d1");

