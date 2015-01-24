#include <rtx_inc.h>
#include <rtx_door.h>
#include <memory.h>

VOID
init_free_memory_queue (free_memory_queue q)
{
  q->head = NULL;
  q->tail = NULL;
}

BOOLEAN
is_free_memory_queue_empty (free_memory_queue q)
{
  return (q->head == NULL);
}

VOID
enqueue_free_memory_queue (free_memory_queue q, memory_block e)
{
  if (is_free_memory_queue_empty (q))
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

memory_block
dequeue_memory_block_FIFO (free_memory_queue q)
{
  memory_block temp;
  memory_block last;

  if (is_free_memory_queue_empty (q))
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


memory_block
dequeue_memory_block_LIFO (free_memory_queue q)
{
  memory_block temp;

  if (is_free_memory_queue_empty (q))
    {
      return NULL;
    }
  else
    {
      temp = q->head;
      q->head = temp->next;
      return temp;
    }
}
