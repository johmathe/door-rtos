#include <rtx_inc.h>
#include <rtx.h>
#include <rtx_door.h>
#include <queue.h>


extern volatile METAPCB_queue READY_queue;
extern BYTE *tmp_sp;
extern volatile UINT32 in_i_process;
UINT32 kernel_clock = 0;

VOID
timer_handler (VOID)
{
  DISABLE_INTERRUPTS ();
  UNLK_FP ();
  SAVE_CONTEXT ();
  GET_SP ();
  current_pcb->stack_pointer = (UINT32 *) tmp_sp;
  in_i_process = 1;
  TIMER0_TER = 2;
  enqueue_PCB_by_priority(READY_queue, (PCB *)current_pcb);
  context_switch ((PCB*)(pcbs + TIMER_PID));
}
