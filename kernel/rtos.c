#include <task.h>
#include <rtx_inc.h>
#include <rtx.h>
#include <init.h>
#include <string.h>
#include <queue.h>
#include <rtx_door.h>
#include <messages.h>
/* Various PCB queues */
volatile struct PCB_queue_t queues[QUEUES_NUMBER+2];

volatile struct message_queue_t m_queues[MAX_PROCESSES];

const PCB_queue INTERRUPTED_queue;

/* Queues of waiting processes */
volatile struct PCB_queue_t queue_MEMORY_WAIT;

/* Here are PCBs */
volatile PCB pcbs[MAX_PROCESSES];
volatile task_init tasks[6];


/*
 * gcc expects this function to exist
 */
int
__main (void)
{
  return 0;
}



int
main (VOID)
{

/* Intialize the free memory queue */
  init_free_memory ();

/* Function related to the processor */
  init_hardware ();

  init_tasks ();

  start_scheduler ();

  return 0;
}
