#include <rtx.h>
#include <rtx_inc.h>
#include <rtx_door.h>

#include <task.h>
#include <string.h>
#include <queue.h>
#include <messages.h>
#include <stdio.h>
#include <memory.h>
#include <arch/m68k-coldfire5307/arch.h>
#include <s_processes.h>
#include <u_processes.h>


volatile BYTE *tmp_sp;
extern volatile PCB *pcb_before_i_process;
extern BYTE __end;
extern volatile UINT32 in_i_process;
extern volatile messages_queue delayed_send_queue;

typedef void (*func) (void *);
volatile PCB *current_pcb;



//free memory queue
struct free_memory_queue_t free_mem_q_struct;
free_memory_queue free_mem_q = &free_mem_q_struct;
UINT32 stack_offset = TOP_OF_KERNEL_STACK;

VOID
init_free_memory (VOID)
{
  int i = 0;
  BYTE *temp;

  //intialize the free memory queue
  init_free_memory_queue (free_mem_q);

  //&__end points to where the code space ends
  //free memory blocks will begin at this location
  temp = &(__end);

  //enqueue NUM_MEM_BLOCKS to the free memory queue
  //NUM_MEM_BLOCKS is a configurable parameter
  while (i < NUM_MEM_BLOCKS)
    {
      //cast temp to a memory_block pointer and enqueue to the free memory queue
      enqueue_free_memory_queue (free_mem_q, (memory_block) temp);
      //increment temp by the size of memory_block
      temp += sizeof (struct memory_block_t);
      i++;
    }
}


VOID
init_hardware (VOID)
{
  DISABLE_INTERRUPTS ();
/* Initialisation of trap 0 in the table*/
  INIT_TRAP_HANDLER ();

}


UINT32 *
init_local_stack (UINT32 * stack_pointer, func proc_entry, BOOLEAN is_i_process)
{
  /* Create a Motorola Coldfire exception stack frame. First comes the return
   * address. */
  *stack_pointer = (UINT32) proc_entry;
  stack_pointer--;

  /* Format, fault-status, vector number for exception stack frame. Task
   * run in supervisor mode. */

if(is_i_process)
{
  *stack_pointer = 0x41002700UL;
}
else
{
    *stack_pointer = 0x41002000UL;
}
  stack_pointer--;

  /* Set the initial critical section nesting counter to zero. This value
   * is used to restore the value of ulCriticalNesting. */

  *stack_pointer = (UINT32) 0xD0;
  stack_pointer--;
  *stack_pointer = (UINT32) 0xD1;
  stack_pointer--;
  *stack_pointer = (UINT32) 0xD2;
  stack_pointer--;
  *stack_pointer = (UINT32) 0xD3;
  stack_pointer--;
  *stack_pointer = (UINT32) 0xD4;
  stack_pointer--;
  *stack_pointer = (UINT32) 0xD5;
  stack_pointer--;
  *stack_pointer = (UINT32) 0xD6;
  stack_pointer--;
  *stack_pointer = (UINT32) 0xD7;
  stack_pointer--;
  *stack_pointer = (UINT32) 0xA0;
  stack_pointer--;
  *stack_pointer = (UINT32) 0xA1;
  stack_pointer--;
  *stack_pointer = (UINT32) 0xA2;
  stack_pointer--;
  *stack_pointer = (UINT32) 0xA3;
  stack_pointer--;
  *stack_pointer = (UINT32) 0xA4;
  stack_pointer--;
  *stack_pointer = (UINT32) 0xA5;
  stack_pointer--;
  *stack_pointer = (UINT32) 0xA6;

  return stack_pointer;


}






VOID
start_scheduler (VOID)
{
  int i;
  int j;
/* Used to set the offset of the place for the stack */

#ifdef DEBUG
  rtx_dbug_outs ("DOOR-KERNEL : Booting : in the scheduler\n\r");
#endif

#ifdef DEBUG
  rtx_dbug_outs ("DOOR-KERNEL : Booting : Queue initialisation...\n\r");
#endif


/* Setting a dummy number for initials pid (max pid in this system) */

  for(i=0;i<MAX_PROCESSES;i++)
  {
    pcbs[i].pid = 100;
  }
/* Here, we are going to NUM_PRIO+1 because we have to handle to possibility of empty queue */

  init_METAPCB_queue ((METAPCB_queue)READY_queue);
  init_METAPCB_queue ((METAPCB_queue)MEMORY_WAIT_queue);


  setTestProcs();

 for (i = 0; i < 6; i++)
    {

   /*  J is going to be the memory ofset for the pcb  */

#ifdef DEBUG
      rtx_dbug_outs ("DOOR-KERNEL : Booting : Init of IRENE's PCB...\n\r");
#endif

      init_msg_queue ((messages_queue) (m_queues + i+1));

/* Init of the pcbs */
      pcbs[i+1].pid =  i+1;
      pcbs[i+1].priority = RTXEVAL_TEST_PROCS[i].priority;
      pcbs[i+1].state = READY;
      pcbs[i+1].stack_pointer = (UINT32 *) (&(__end) + stack_offset);
      pcbs[i+1].stack_size = RTXEVAL_TEST_PROCS[i].stacksize;
      stack_offset -= pcbs[i+1].stack_size;
      pcbs[i+1].rtx_proc_entry = RTXEVAL_TEST_PROCS[i].rtxeval_proc_entry;
      pcbs[i+1].mess_queue = (messages_queue) (m_queues + i+1);

/* putting init function */
      pcbs[i+1].stack_pointer = init_local_stack ((UINT32 *) pcbs[i+1].stack_pointer, RTXEVAL_TEST_PROCS[i].rtxeval_proc_entry,0);
/* Enqueue all the PCBS in the ready queue */

      enqueue_PCB_by_priority ((METAPCB_queue)READY_queue,(PCB*) (pcbs +  i+1));

    }




in_i_process = 0;


#ifdef DEBUG
  rtx_dbug_outs ("DOOR-KERNEL : Booting : Dequeuing first PCB...\n\r");
#endif

  current_pcb = dequeue_PCB_FIFO_by_priority (READY_queue);
  pcb_before_i_process = current_pcb;
/* Starting the rtos */

#ifdef DEBUG
  rtx_dbug_outs ("DOOR-KERNEL : Booting : calling the first process  ..\n\r");
#endif

/* Starting the first process */
  tmp_sp = (BYTE *) current_pcb->stack_pointer;
  SET_SP ();
  RESTORE_CONTEXT ();
/* Enabling interrupts is useless, rte is doing this automatically */
  RETURN_FROM_EXCEPTION ();
}
