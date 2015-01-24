#ifndef __RTX_DOOR_H__
#define __RTX_DOOR_H__
#include <rtx.h>
#include <task.h>
#include <s_processes.h>
#include <arch/m68k-coldfire5307/arch.h>

#define WAIT1SEC 100

/* Pid of the processes */
#define TIMER_PID 0x30
#define UART_PID 0x31

/* Table index for the processes */
#define TIMER_ID 0
#define UART_ID 1

#define DEFAULT_STACK_SIZE 512 
#define TOP_OF_KERNEL_STACK 0xB0000 


/* Number of clock ticks for 1 sec delay */
#define DELAY_1SEC 10


#define NULL_PRIORITY 4
/* Kernel primitives */
void *kernel_request_memory_block (VOID);
VOID context_switch (PCB * next_pcb);
VOID process_switch (VOID);
void *kernel_request_memory_block (VOID);
int kernel_release_memory_block (void *MemoryBlock);
int kernel_delayed_send(int process_ID, void * MessageEnvelope, int delay);
int kernel_get_process_priority (int process_ID);
int kernel_set_process_priority (UINT32 process_ID, UINT8 priority);


VOID timer_handler (VOID);
VOID timer_i_process (VOID);
VOID uart_i_process (VOID);



int kernel_delayed_send(int process_ID, void * MessageEnvelope, int delay);
VOID *kernel_receive_message (UINT32 * sender_ID);
UINT32 kernel_send_message (UINT32 process_ID, VOID * MessageEnvelope);

#endif
