/*  
 *
 * this file contains task related informations
 * for the ece354 DOOR rtoss
 *
 * Under GPLv2
 *
 *
 */
#ifndef __TASK_H__
#define __TASK_H__
#include <rtx_inc.h>
#include <rtx.h>
#include <messages.h>

#define MAX_PROCESSES 0x40 


enum states
{
  READY = 0,
  MESSAGE_WAIT,
  MEMORY_WAIT,
  RUNNING
};

typedef struct task_init_t
{
  UINT32 pid;			//pid of test process
  UINT8 priority;		//priority of test process
  UINT32 stacksize;		//stack size of test process
  VOID (*rtxeval_proc_entry) ();	//entry point of test process
} task_init;

typedef struct PCB_t
{
  UINT32 pid;
  UINT8 state;
  UINT8 priority;
  UINT32 *stack_pointer;
  UINT32 stack_size;
  messages_queue mess_queue;
  VOID (*rtx_proc_entry) ();
  struct PCB_t *next;
} PCB;


extern volatile PCB pcbs[MAX_PROCESSES];
extern volatile task_init tasks[6];

extern volatile PCB *current_pcb;

/* Various functions about PCBS */
PCB *get_pcb (UINT32 pid);


#endif
