#include <rtx.h>
#include <rtx_door.h>
#include <rtx_inc.h>
#include <stdio.h>
//Author: Gohulan Balachandran
//Description: Software Interrupts
extern volatile BYTE *tmp_sp;
volatile UINT32 kernel_code = 0;
volatile UINT32 param_1 = 0;
volatile UINT32 param_2 = 0;
volatile UINT32 param_3 = 0;
volatile UINT32 ret_code_recv = 0;
volatile UINT32 ret_code_get_prio = 0;
volatile UINT32 ret_code = 0;
volatile UINT32 msg_return = 0;

void
swi_trap_handler (VOID)
{
  DISABLE_INTERRUPTS ();
  UNLK_FP ();
  SAVE_CONTEXT ();
  GET_SP ();
  switch (kernel_code)
    {
    case SEND_MESSAGE:
      ret_code = kernel_send_message (param_1, (VOID *) param_2);
    break;

    case RECEIVE_MESSAGE:
      kernel_receive_message ((UINT32 *) param_1);
    break;

    case REQUEST_MEMORY:
         ret_code = (UINT32) kernel_request_memory_block();
    break;

    case RELEASE_MEMORY:
        ret_code = kernel_release_memory_block((void*)param_1);
    break;

    case RELEASE_PROCESSOR:
           process_switch ();
    break;

    case DELAYED_SEND:
        ret_code =  kernel_delayed_send((int)param_1,(void*)param_2,(int)param_3);
    break;
    case SET_PRIO:
        ret_code =  kernel_set_process_priority(param_1,param_2);
    break;
    case GET_PRIO:
         kernel_get_process_priority(param_1);
    break;
    default:
    break;
    }

  SET_SP ();
  RESTORE_CONTEXT ();
  RETURN_FROM_EXCEPTION ();
}

/* Interprocess Communications*/

int
release_processor ()
{
  kernel_code = RELEASE_PROCESSOR;
  TRAP0 ();
  return 0;
}


int
send_message (int process_ID, void *MessageEnvelope)
{
  param_1 = process_ID;
  param_2 = (UINT32) MessageEnvelope;
  kernel_code = SEND_MESSAGE;


  TRAP0 ();
  return ret_code;
}

void *
receive_message (int *Senderid)
{

  msg_return = NULL;
  ret_code_recv = NULL;
  param_1 = Senderid;
  while (!msg_return)
    {
      kernel_code = RECEIVE_MESSAGE;
      TRAP0 ();
    }
  return (void *) ret_code_recv;
}


/*Memory Management*/

void * request_memory_block(VOID){
    kernel_code = REQUEST_MEMORY;
    TRAP0();
	return (void*)ret_code;
}

int release_memory_block(void * MemoryBlock){
	param_1 = (UINT32)MemoryBlock;
    kernel_code = RELEASE_MEMORY;
    TRAP0();
	return ret_code;
}

/*Timing Service*/

int delayed_send(int process_ID, void * MessageEnvelope, int delay){
    param_1 = process_ID;
    param_2 = (UINT32) MessageEnvelope;
    param_3 = delay;
    kernel_code = DELAYED_SEND;
    TRAP0 ();
	return ret_code;
}


/*Process Management*/

int set_process_priority (int process_ID, int priority){
   param_1 = process_ID;
   param_2 = priority;
   kernel_code = SET_PRIO;
   TRAP0 ();
   return ret_code_get_prio;
}


int get_process_priority (int process_ID){
    param_1 = process_ID;
    kernel_code = GET_PRIO;
    TRAP0 ();
   	return ret_code_get_prio;
}
