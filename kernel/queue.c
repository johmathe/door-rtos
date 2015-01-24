#include <task.h>
#include <queue.h>
#include <stdio.h>
#undef QUEUE_DEBUG
#define DEBUG

struct METAPCB_queue_t READY_queue_struct;
struct METAPCB_queue_t MEMORY_WAIT_queue_struct;

volatile METAPCB_queue READY_queue = &READY_queue_struct;
volatile METAPCB_queue MEMORY_WAIT_queue = &MEMORY_WAIT_queue_struct;
VOID
init_PCB_queue (PCB_queue q)
{
  q->head = NULL;
  q->tail = NULL;
}


VOID
init_METAPCB_queue (METAPCB_queue q)
{
  UINT8 i;
  q->highest_prio = NUM_PRIO + 1;
/* Init each queue for each priority */
  for (i = 0; i < NUM_PRIO + 1; i++)
    {
      init_PCB_queue (q->subqueues + i);
    }
}

BOOLEAN
isEmpty (PCB_queue q)
{
  return (q->head == NULL);
}

BOOLEAN
is_METAPCB_queue_empty (METAPCB_queue q)
{
  if(q->highest_prio > NUM_PRIO)
  {
    return 1;
  }
  return (isEmpty (q->subqueues + q->highest_prio));
}

VOID
enqueue_PCB (PCB_queue q, PCB * e)
{
  if (isEmpty (q))
    {
      q->head = e;
      q->tail = e;
      e->next = NULL;
    }
  else
    {
      e->next = q->head;
      q->head = e;
    }
}


PCB *
dequeue_PCB_FIFO (PCB_queue q)
{
  PCB *temp;
  PCB *last;

  if (isEmpty (q))
    {
      return NULL;
    }
  else
    {

      last = q->head;
      temp = q->tail;

      if (last->next == NULL)
	{			//if there is only one element in the queue
	  q->head = NULL;
	  q->tail = NULL;
	}
      else
	{
	  while (last->next != temp)
	    {			//loop till the second last element is reached
	      last = last->next;
	    }

	  last->next = NULL;
	  q->tail = last;

	}
      return temp;
    }
}

PCB *
dequeue_PCB_LIFO (PCB_queue q)
{
  PCB *temp;

  if (isEmpty (q))
    {
      return NULL;
    }
  else
    {
      temp = (PCB *) q->head;
      q->head = temp->next;
      return temp;
    }
}

PCB *
searchDequeue (PCB_queue q, PCB * e)
{

  PCB *temp;
  PCB *dequeued;

  temp = (PCB *) (q->head);

  if ((q->head) == e)
    {
      return dequeue_PCB_FIFO (q);
    }
  else if ((q->tail) == e)
    {
      return dequeue_PCB_LIFO (q);
    }

  while ((temp->next) != NULL)
    {
      if ((temp->next) == e)
	{
	  dequeued = temp->next;
	  temp->next = (temp->next)->next;
	  return dequeued;
	}
      temp = temp->next;
    }

  return NULL;

}

BOOLEAN
search (PCB_queue q, PCB * e)
{

  PCB *temp;

  temp = (PCB *) (q->head);

  if ((q->head) == e)
    {
      return TRUE;
    }
  else if ((q->tail) == e)
    {
      return TRUE;
    }

  while ((temp->next) != NULL)
    {
      if ((temp->next) == e)
	{
	  return TRUE;
	}
      temp = temp->next;
    }

  return FALSE;

}


PCB *
dequeue_PCB_FIFO_by_priority (METAPCB_queue q)
{
/* Always think than the highest prio is zero */
  PCB *tmp_pcb;
  tmp_pcb = NULL;

#ifdef QUEUE_DEBUG
  rtx_dbug_outs ("DOOR-KERNEL :  DEQUEUING (hiq : ");
  rtx_dbug_number (q->highest_prio);
  rtx_dbug_outs (") \n\r");
#endif

  if (q->highest_prio < (NUM_PRIO + 1))
    {
#ifdef QUEUE_DEBUG
      rtx_dbug_outs ("DOOR-KERNEL :  PCB FOUND ! dequeuing fifo (hiq : ");
      rtx_dbug_number (q->highest_prio);
      rtx_dbug_outs (" )\n\r");

#endif

      tmp_pcb = dequeue_PCB_FIFO (q->subqueues + q->highest_prio);

      if (tmp_pcb == NULL)
	{
#ifdef QUEUE_DEBUG
	  rtx_dbug_outs
	    ("DOOR-KERNEL :  PROBLEM : NULL pcb when dequeuing queue !\n\r");
#endif
	}
      /* In the worse case, highest priority in queue becomes NUM +1, which means that the whole queue is empty */
      while (isEmpty (q->subqueues + q->highest_prio)
	     && q->highest_prio < (NUM_PRIO + 1))
	{
#ifdef QUEUE_DEBUG
	  rtx_dbug_outs
	    ("DOOR-KERNEL :  EMPTY QUEUE : increasing highest priority !\n\r");
#endif

	  q->highest_prio++;
	}
    }
#ifdef QUEUE_DEBUG
  rtx_dbug_outs ("DOOR-KERNEL : PROCESS PID : ");
  rtx_dbug_number (tmp_pcb->pid);
  rtx_dbug_outs (" \n\r");
#endif
  return tmp_pcb;
}

VOID
enqueue_PCB_by_priority (METAPCB_queue q, PCB * pcb)
{

#ifdef QUEUE_DEBUG
  rtx_dbug_outs ("DOOR-KERNEL : PROCESS PID : ");
  rtx_dbug_number (pcb->pid);
  rtx_dbug_outs (" \n\r");
#endif


  enqueue_PCB (q->subqueues + pcb->priority, pcb);

  if (pcb->priority < q->highest_prio)
    {

#ifdef QUEUE_DEBUG
      rtx_dbug_outs
	("DOOR-KERNEL : Changing new highest prio in queue : before : ");
      rtx_dbug_number (q->highest_prio);
#endif

      q->highest_prio = pcb->priority;

#ifdef QUEUE_DEBUG
      rtx_dbug_outs (" after : ");
      rtx_dbug_number (q->highest_prio);
      rtx_dbug_outs ("\n\r");
#endif

    }

#ifdef QUEUE_DEBUG
  rtx_dbug_outs ("DOOR-KERNEL :  ENQUEUING with priority ");
  rtx_dbug_number (pcb->priority);
  rtx_dbug_outs ("\n\r");
#endif


}


BOOLEAN remove_element_by_priority (PCB* pcb, METAPCB_queue q){
   return(remove_element(q,q->subqueues + pcb->priority,pcb));
}

BOOLEAN remove_element (METAPCB_queue mq, PCB_queue q, PCB * pcb){
    volatile PCB *current;
    volatile PCB *previous;

    //if pcb is null, return error
    if (pcb == NULL){
        return RTX_ERROR;
    }

    current = q->head;
        
    while (current != pcb && current != NULL){
        previous = current;
        current = current->next;
    }

    //if pcb is not found and/or the queue is empty
    if (current == NULL){
        return RTX_ERROR;
    }

    //if pcb is the only one in the queue
    else if (current == q->head && current == q->tail){
        q->head = NULL;
        q->tail = NULL;

        while (isEmpty (mq->subqueues + mq->highest_prio) && mq->highest_prio < (NUM_PRIO + 1))
        {
//#ifdef QUEUE_DEBUG
                rtx_dbug_outs("DOOR-KERNEL : REMOVEELEMENT : EMPTY QUEUE : increasing highest priority !\n\r");
//#endif

                mq->highest_prio++;
        }
    }

    //if pcb is at the head (the first element) but there is more than one pcb in the queue
    else if (current == q->head){
        q->head = current->next;
    }

    //if pcb is at the tail (the last element) but there is more than once pcb in the queue
    else if (current == q->tail){
        q->tail =(PCB*) previous;
    }

    //if pcb is elsewhere in the queue
    else if (current == pcb){
        previous->next = current->next;
    }

    return RTX_SUCCESS;
 
}


