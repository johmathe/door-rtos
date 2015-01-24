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
init_timer (VOID)
{
  UINT32 mask;

  INIT_TIMER_INTERRUPT ();

  /*
   * Setup to use auto-vectored interupt level 6, priority 3
   */
  TIMER0_ICR = 0x9B;

  /*
   * Set the reference counts, ~10ms
   */
  TIMER0_TRR = 1758;

  /*
   * Setup the timer prescaler and stuff
   */
  TIMER0_TMR = 0xFF1B;

  /*
   * Set the interupt mask
   */
  mask = SIM_IMR;
  mask &= 0x0003fdff;
  SIM_IMR = mask;

}


VOID init_UART (VOID) {

    UINT32 mask;

    INIT_UART_INTERRUPT();

    /* Reset the entire UART */
    SERIAL1_UCR = 0x10;

    /* Reset the receiver */
    SERIAL1_UCR = 0x20;

    /* Reset the transmitter */
    SERIAL1_UCR = 0x30;

    /* Reset the error condition */
    SERIAL1_UCR = 0x40;

    /* Install the interupt */
    SERIAL1_ICR = 0x17;
    SERIAL1_IVR = 64;

    /* enable interrupts on rx only */
    SERIAL1_IMR = 0x02;

    /* Setup for transmit and receive */
    SERIAL1_UCR = 0x05;

    /* Enable interupts */
    mask = SIM_IMR;
    mask &= 0x0003dfff;
    SIM_IMR = mask;

}


VOID
init_hardware (VOID)
{
  DISABLE_INTERRUPTS ();
/* Initialisation of trap 0 in the table*/
  INIT_TRAP_HANDLER ();

  init_timer ();

 init_UART ();

}


UINT32 *
init_local_stack (UINT32 * stack_pointer, func proc_entry, BOOLEAN is_i_process)
{
/* First, return adress */

  *stack_pointer = (BYTE*) proc_entry;
  stack_pointer--;


if(is_i_process)
{
  *stack_pointer = 0x41002700UL;
}
else
{
/* Here, local processes are running in user mode (410000000) */
    *stack_pointer = 0x41000000UL;
}
  stack_pointer--;


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
init_i_processes (VOID)
{
in_i_process  = 0;
/* Init of the timer i-process */
  init_msg_queue ((messages_queue) m_queues + TIMER_PID);
  pcbs[TIMER_PID].pid = TIMER_PID;
  pcbs[TIMER_PID].priority = LOW;
  pcbs[TIMER_PID].state = READY;
  pcbs[TIMER_PID].stack_pointer =  (BYTE *) ((&(__end) + stack_offset));
  pcbs[TIMER_PID].stack_size = DEFAULT_STACK_SIZE;
  stack_offset -= pcbs[TIMER_PID].stack_size;
  pcbs[TIMER_PID].rtx_proc_entry = timer_i_process;
  pcbs[TIMER_PID].stack_pointer = init_local_stack ((BYTE *) pcbs[TIMER_PID].stack_pointer, pcbs[TIMER_PID].rtx_proc_entry,1);
  pcbs[TIMER_PID].mess_queue =  (messages_queue) m_queues + TIMER_PID;

  init_msg_queue(delayed_send_queue);


/* Init of the uart i-process */
  init_msg_queue ((messages_queue) m_queues + UART_PID);
  pcbs[UART_PID].pid = UART_PID;
  pcbs[UART_PID].priority = LOW;
  pcbs[UART_PID].state = READY;
  pcbs[UART_PID].stack_pointer = (BYTE *) (&(__end) + stack_offset);
  pcbs[UART_PID].stack_size = DEFAULT_STACK_SIZE;
  stack_offset -= pcbs[UART_PID].stack_size;
  pcbs[UART_PID].rtx_proc_entry = uart_i_process;
  pcbs[UART_PID].stack_pointer = init_local_stack ((BYTE *) pcbs[UART_PID].stack_pointer,pcbs[UART_PID].rtx_proc_entry,1);
  pcbs[UART_PID].mess_queue =  (messages_queue) m_queues + UART_PID;

}


VOID 
init_s_processes(VOID)
{
init_msg_queue ((messages_queue) m_queues + KCD_PID);
pcbs[KCD_PID].pid = KCD_PID;
pcbs[KCD_PID].priority = MEDIUM;
pcbs[KCD_PID].stack_pointer =  (BYTE *) (&(__end) + stack_offset);
pcbs[KCD_PID].state = READY;
pcbs[KCD_PID].stack_size = 2048;
stack_offset -= pcbs[KCD_PID].stack_size;
pcbs[KCD_PID].rtx_proc_entry = kcd_process;
pcbs[KCD_PID].stack_pointer = init_local_stack ((BYTE *) pcbs[KCD_PID].stack_pointer, pcbs[KCD_PID].rtx_proc_entry,0);
pcbs[KCD_PID].mess_queue =  (messages_queue) m_queues + KCD_PID;
enqueue_PCB_by_priority ((METAPCB_queue)READY_queue,(PCB*) (pcbs+KCD_PID));

/* Init of the KCD process */

init_msg_queue ((messages_queue) m_queues + WC_PID);
pcbs[WC_PID].pid = WC_PID;
pcbs[WC_PID].priority = HIGH;
pcbs[WC_PID].stack_pointer =  (BYTE *) (&(__end) + stack_offset);
pcbs[WC_PID].state = READY;
pcbs[WC_PID].stack_size = 2048;
stack_offset -= pcbs[WC_PID].stack_size;
pcbs[WC_PID].rtx_proc_entry = wall_clock_process;
pcbs[WC_PID].stack_pointer = init_local_stack ((BYTE *) pcbs[WC_PID].stack_pointer, pcbs[WC_PID].rtx_proc_entry,0);
pcbs[WC_PID].mess_queue =  (messages_queue) m_queues + WC_PID;
enqueue_PCB_by_priority ((METAPCB_queue)READY_queue,(PCB*) (pcbs+WC_PID));


init_msg_queue ((messages_queue) m_queues + CRT_PID);
pcbs[CRT_PID].pid = CRT_PID;
pcbs[CRT_PID].priority = HIGH;
pcbs[CRT_PID].stack_pointer =  (BYTE *) (&(__end) + stack_offset);
pcbs[CRT_PID].state = READY;
pcbs[CRT_PID].stack_size = 2048;
stack_offset -= pcbs[CRT_PID].stack_size;
pcbs[CRT_PID].rtx_proc_entry = crt_process;
pcbs[CRT_PID].stack_pointer = init_local_stack ((BYTE *) pcbs[CRT_PID].stack_pointer, pcbs[CRT_PID].rtx_proc_entry,0);
pcbs[CRT_PID].mess_queue =  (messages_queue) m_queues + CRT_PID;
enqueue_PCB_by_priority ((METAPCB_queue)READY_queue,(PCB*) (pcbs+CRT_PID));

init_msg_queue ((messages_queue) m_queues + SET_PRIO_PID);
pcbs[SET_PRIO_PID].pid = SET_PRIO_PID;
pcbs[SET_PRIO_PID].priority = HIGH;
pcbs[SET_PRIO_PID].stack_pointer =  (BYTE *) (&(__end) + stack_offset);
pcbs[SET_PRIO_PID].state = READY;
pcbs[SET_PRIO_PID].stack_size = 2048;
stack_offset -= pcbs[SET_PRIO_PID].stack_size;
pcbs[SET_PRIO_PID].rtx_proc_entry = set_prio_process;
pcbs[SET_PRIO_PID].stack_pointer = init_local_stack ((BYTE *) pcbs[SET_PRIO_PID].stack_pointer, pcbs[SET_PRIO_PID].rtx_proc_entry,0);
pcbs[SET_PRIO_PID].mess_queue =  (messages_queue) m_queues + SET_PRIO_PID;
enqueue_PCB_by_priority ((METAPCB_queue)READY_queue,(PCB*) (pcbs+SET_PRIO_PID));

init_msg_queue ((messages_queue) m_queues + NULL_PID);
pcbs[NULL_PID].pid = NULL_PID;
pcbs[NULL_PID].priority = NULL_PRIORITY;
pcbs[NULL_PID].stack_pointer =  (BYTE *) (&(__end) + stack_offset);
pcbs[NULL_PID].state = READY;
pcbs[NULL_PID].stack_size = DEFAULT_STACK_SIZE;
stack_offset -= pcbs[NULL_PID].stack_size;
pcbs[NULL_PID].rtx_proc_entry = null_process;
pcbs[NULL_PID].stack_pointer = init_local_stack ((BYTE *) pcbs[NULL_PID].stack_pointer, pcbs[NULL_PID].rtx_proc_entry,0);
pcbs[NULL_PID].mess_queue =  (messages_queue) m_queues + NULL_PID;
enqueue_PCB_by_priority ((METAPCB_queue)READY_queue,(PCB*) (pcbs+NULL_PID));


}





int 
init_process(UINT32 pid, func func_point)
{

init_msg_queue ((messages_queue) m_queues + pid);
pcbs[pid].pid = pid;
pcbs[pid].priority = MEDIUM;
pcbs[pid].stack_pointer =  (UINT32 *) (&(__end) + stack_offset);
pcbs[pid].state = READY;
pcbs[pid].stack_size = DEFAULT_STACK_SIZE;
stack_offset -= pcbs[pid].stack_size;
pcbs[pid].rtx_proc_entry = func_point;
pcbs[pid].stack_pointer = init_local_stack ((UINT32 *) pcbs[pid].stack_pointer, pcbs[pid].rtx_proc_entry,0);
pcbs[pid].mess_queue =  (messages_queue) m_queues + pid;
enqueue_PCB_by_priority ((METAPCB_queue)READY_queue,(PCB*) (pcbs+pid));

return 0;
}



VOID init_stress_processes()
{
 init_process(PROCESS_A1, process_a1);
 init_process(PROCESS_B1, process_b1);
 init_process(PROCESS_C1, process_c1);
 init_process(PROCESS_A2, process_a2);
 init_process(PROCESS_B2, process_b2);
 init_process(PROCESS_C2, process_c2);
 init_process(PROCESS_A3, process_a3);
}







VOID
start_scheduler (VOID)
{
  int i;
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


#if defined (_RTX_EVAL_)
setTestProcs();

 for (i = 0; i < 6; i++)
    {

   /*  J is going to be the memory ofset for the pcb  */

#ifdef DEBUG
      rtx_dbug_outs ("DOOR-KERNEL : Booting : Init of IRENNNNE PCB...\n\r");
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




#else


  for (i = 1; i < 4; i++)
    {
#ifdef DEBUG
      rtx_dbug_outs ("DOOR-KERNEL : Booting : Init of PCB...\n\r");
#endif

      init_msg_queue ((messages_queue) (m_queues + i));

/* Init of the pcbs */
      pcbs[i].pid = i;
      pcbs[i].priority = tasks[i].priority;
      pcbs[i].state = READY;
      pcbs[i].stack_pointer = (BYTE *) (&(__end) + stack_offset);
      pcbs[i].stack_size = tasks[i].stacksize;
      stack_offset -= pcbs[i].stack_size;
      pcbs[i].rtx_proc_entry = tasks[i].rtxeval_proc_entry;
      pcbs[i].mess_queue = (messages_queue) (m_queues + i);
/* putting init function */
      pcbs[i].stack_pointer =
	init_local_stack ((UINT32 *) pcbs[i].stack_pointer,
			  tasks[i].rtxeval_proc_entry,0);
/* Enqueue all the PCBS in the ready queue */

      enqueue_PCB_by_priority ((METAPCB_queue)READY_queue,(PCB*) (pcbs + i));


    }

#endif

  rtx_dbug_outs ("DOOR-KERNEL : Booting : Init of U-STRESS-PROCESSES...\n\r");
  init_stress_processes();

  rtx_dbug_outs ("DOOR-KERNEL : Booting : Init of S-PROCESSES...\n\r");
  init_s_processes();

  rtx_dbug_outs ("DOOR-KERNEL : Booting : Init of I-PROCESSES...\n\r");
  init_i_processes ();

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
