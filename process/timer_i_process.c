#include <rtx_inc.h>
#include <rtx.h>
#include <stdio.h>

#include <rtx_door.h>
#include <messages.h>

#undef DBUG_TIMER
#undef MSG_DEBUG
UINT32 kernel_clock;
volatile UINT32 in_i_process;
volatile UINT32 timer_count;
extern volatile messages_queue delayed_send_queue;
extern volatile UINT32 msg_return;

VOID timer_i_process(VOID)
{
volatile UINT32 timer_count=0;
volatile  mess* env_ptr = NULL;
volatile   UINT32 delay=0;
volatile  mess* ptr = NULL ;
volatile  mess* prev_ptr = NULL;

volatile  UINT32 temp_id=0;
volatile  UINT32 sender_id=0;
kernel_clock = 0;
timer_count = 0;
  

  while(1)
  {
     
    kernel_clock++;
    timer_count++;

    env_ptr = (mess*)kernel_receive_message(NULL);
    msg_return = 0;
    while(env_ptr != NULL)
    {

      
 
      delay = env_ptr->delay;
      if (delay == 0){
        rtx_dbug_outs("KERNEL PANIC\n\r");
//        return kernel_send_message(1, MessageEnvelope);
      }

      ptr = delayed_send_queue->head;
      prev_ptr = NULL;

      while (ptr != NULL && delay > ptr->delay) {
//        rtx_dbug_outs("TIMER I PROCESS : seeking queue \n\r");
        delay = delay -  ptr->delay;
        prev_ptr = ptr;
        ptr = ptr->next;
      }

      temp_id = env_ptr->sender_pid;
      env_ptr->dest_pid = env_ptr->sender_pid;
      env_ptr->sender_pid = temp_id;

#ifdef MSG_DEBUG
//      rtx_dbug_outs("send pid && dest pid :");
//      rtx_print_integer(env_ptr->dest_pid);
//      rtx_print_integer(env_ptr->sender_pid);
#endif

      //account for the case when the delayed send queue is empty
      if (ptr == NULL && prev_ptr == NULL)
      {
//       rtx_dbug_outs("TIMER-I-PROCESS : Empty queue detected !\n\r");
        env_ptr->delay = delay;
  //      rtx_dbug_outs("TIMER-I-PROCESS : Enqueueing message \n\r");
        enqueue_message(delayed_send_queue,(mess*)env_ptr);
      }
      //account for the case when delay is smaller than the first message envelope's delay
      else if (prev_ptr == NULL)
      {
//        rtx_dbug_outs("TIMER-I-PROCESS : Putting element at the beginning potentially useless this time\n\r");
        ptr->delay = ptr->delay - delay;
        env_ptr->delay = delay;
        env_ptr->next = (mess*) ptr;
        delayed_send_queue->head = (mess*) env_ptr;
      }
      //account for the case when delay is larger than the last message envelope's delay
      else if (ptr == NULL)
      {
  //      rtx_dbug_outs("TIMER-I-PROCESS : Larger than the last message envelope's delay. New delay :");
        env_ptr->delay = delay;
   //     rtx_print_integer(delay);
//        rtx_dbug_outs("\n\r");
        env_ptr->next = NULL;
        prev_ptr->next = (mess*) env_ptr;
      }
      else
      {
//       rtx_dbug_outs("TIMER-I-PROCESS : Between two elements\n\r ");
        ptr->delay = ptr->delay - delay;
        env_ptr->delay = delay;
        prev_ptr->next = (mess*) env_ptr;
        env_ptr->next = (mess*) ptr;
      }

    env_ptr = (mess*)kernel_receive_message((UINT32*)&sender_id);
    msg_return = 0;
    }

    env_ptr = (mess*)delayed_send_queue->head;
#ifdef DBUG_TIMER
//    rtx_dbug_outs("\n\rtim value : ");
//    rtx_print_integer(timer_count);
#endif 
    if(env_ptr != NULL && timer_count > env_ptr->delay)
    {
        timer_count = 0;
    }

    while(env_ptr != NULL && env_ptr->delay == timer_count)
    {
     timer_count = 0;
//      rtx_dbug_outs("\n\r--TIMER RELEASED--\n\r");
      env_ptr = dequeueMessageLIFO(delayed_send_queue);
      kernel_send_message(env_ptr->dest_pid, (mess*)env_ptr);
      env_ptr = (mess*)delayed_send_queue->head;
    }
    release_processor();
    DISABLE_INTERRUPTS();
  
  }
    
}

