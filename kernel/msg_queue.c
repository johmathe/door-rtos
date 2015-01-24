#include <rtx_door.h>
#include <task.h>
#include <messages.h>
#include <stdio.h>


#undef MSGQ_DEBUG
volatile struct message_queue_t delayed_send_queue_s;

volatile messages_queue delayed_send_queue =
  (messages_queue) & delayed_send_queue_s;

VOID
init_msg_queue (messages_queue q)
{
  q->head = NULL;
  q->tail = NULL;
}


BOOLEAN
is_msgq_empty (messages_queue q)
{
  return (q->head == NULL);
}

/* Message queues functions */

VOID
enqueue_message (messages_queue q, mess * e)
{
  if (is_msgq_empty (q))
    {
      q->head = e;
      q->tail = e;
      e->next = NULL;
    }
  else
    {
      e->next = (mess *) (q->head);
      q->head = e;
    }
}


mess *
dequeueMessageLIFO (messages_queue q)
{
  mess *temp;
  if (is_msgq_empty (q))
    {
      return NULL;
    }
  else
    {
      temp = (mess *) (q->head);
      q->head = temp->next;
      return temp;
    }

}
mess *
dequeueMessageFIFO (messages_queue q)
{
#ifdef MFGQ_DEBUG
  rtx_dbug_outs ("DOOR-KERNEL : in DEQUEUE FIFO MESSAGE\n\r");
#endif
  mess *temp;
  mess *last;

  if (is_msgq_empty (q))
    {
#ifdef MFGQ_DEBUG
      rtx_dbug_outs ("DOOR-KERNEL : the message queue is empty\r\n");
#endif

      return NULL;
    }
  else
    {

      last = q->head;
      temp = q->tail;

      if ((last->next) == NULL)
	{			//if there is only one element in the queue
#ifdef MFGQ_DEBUG
	  rtx_dbug_outs
	    ("DOOR-KERNEL : message queue only has 1 element.\n\r");
#endif

	  q->head = NULL;
	  q->tail = NULL;
	}
      else
	{
	  while ((last->next) != temp)
	    {			//loop till the second last element is reached
	      last = last->next;
	    }

	  last->next = NULL;
	  q->tail = last;

	}
      return temp;
    }
}

mess *
searchDequeueMessage (messages_queue q, mess * e)
{

  mess *temp;
  mess *dequeued;

  temp = (mess *) q->head;

  if ((q->head) == e)
    {
      return dequeueMessageFIFO (q);
    }
  else if ((q->tail) == e)
    {
      return dequeueMessageLIFO (q);
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
