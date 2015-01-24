#ifndef __QUEUE_H__
#define __QUEUE_H__
#include <rtx_inc.h>
#include <rtx_door.h>
#include <messages.h>
#include <memory.h>
#include <task.h>

#define QUEUES_NUMBER 5
/* this global variable is defined in rtos.c */

typedef struct PCB_queue_t
{
  PCB *head;
  PCB *tail;
} *PCB_queue;


typedef struct METAPCB_queue_t
{
  UINT32 highest_prio;
  struct PCB_queue_t subqueues[NUM_PRIO + 1];
} *METAPCB_queue;


//PCB Queue functions
VOID init_PCB_queue (PCB_queue q);
VOID enqueue_PCB (PCB_queue q, PCB * e);
PCB *dequeue_PCB_FIFO (PCB_queue q);


BOOLEAN remove_element_by_priority (PCB* pcb, METAPCB_queue q);
BOOLEAN remove_element (METAPCB_queue mq, PCB_queue q, PCB * pcb);



// META queues functions
VOID init_METAPCB_queue (METAPCB_queue q);
VOID enqueue_PCB_by_priority (METAPCB_queue q, PCB * e);
PCB *dequeue_PCB_FIFO_by_priority (METAPCB_queue q);
BOOLEAN is_METAPCB_queue_empty (METAPCB_queue q);


//free memory functions
VOID init_free_memory_queue (free_memory_queue q);
BOOLEAN is_free_memory_queue_empty (free_memory_queue q);
VOID enqueue_free_memory_queue (free_memory_queue q, memory_block e);
memory_block dequeue_memory_block_FIFO (free_memory_queue q);
memory_block dequeue_memory_block_LIFO (free_memory_queue q);

extern volatile struct PCB_queue_t queue_MESSAGE_WAIT;

extern volatile METAPCB_queue READY_queue;
extern volatile METAPCB_queue MEMORY_WAIT_queue;
extern volatile struct message_queue_t m_queues[MAX_PROCESSES];

extern UINT8 highest_prio_in_queue;

#endif
