#include <rtx.h>
#include <rtx_inc.h>
#include <rtx_door.h>
#include <task.h>
#include <queue.h>
#include <stdio.h>
#include <messages.h>
/* Interprocess Communications*/  
extern volatile BYTE *tmp_sp;
extern volatile PCB *current_pcb;
extern volatile PCB *pcb_before_i_process;
extern volatile UINT32 ret_code;
extern volatile UINT32 ret_code_recv;
extern volatile UINT32 msg_return;
extern volatile UINT32 ret_code_get_prio;
extern volatile UINT32 in_i_process;
extern volatile messages_queue delayed_send_queue;


VOID context_switch (PCB * next_pcb) 
{
        current_pcb->stack_pointer = (UINT32 *) tmp_sp;
        current_pcb = next_pcb;
        tmp_sp = (BYTE *) current_pcb->stack_pointer;
        SET_SP ();
        RESTORE_CONTEXT ();
        /* Enable intterupts is useless, rte is doing this automatically */ 
        RETURN_FROM_EXCEPTION ();
} 


VOID process_switch () 
{
        PCB * next_pcb;
        if (READY_queue->highest_prio <= current_pcb->priority)
        {

                /* grab the next ready process */ 
                next_pcb = dequeue_PCB_FIFO_by_priority (READY_queue);

                /* Checking if FIFO is empty */ 
                if (next_pcb == NULL)
                {
                ret_code = RTX_ERROR;
                }

                /* set the next pcb state to running */ 
                next_pcb->state = RUNNING;

                /* make the current pcb state set to "Ready" */ 
                current_pcb->state = READY;

                /* Enqueue the current PCB in the ready queue */ 
                enqueue_PCB_by_priority (READY_queue, (PCB *) current_pcb);

                /* Context switch to the next process */ 

                if(in_i_process)
                {
                    in_i_process = 0;
                }

               context_switch (next_pcb);
        }
}

UINT32 kernel_send_message (UINT32 process_ID, VOID * message_envelope) 
{
        volatile UINT32 sourcePID, destinationPID;
        volatile PCB * destinationPCB;
        volatile PCB * sourcePCB;
        volatile PCB * next_pcb;

        sourcePID = current_pcb->pid;
        destinationPID = process_ID;


        ((mess*)message_envelope)->sender_pid = sourcePID;
        ((mess*)message_envelope)->dest_pid = destinationPID;

        //  rtx_dbug_outs("Sending message...\n\r");
        //  rtx_print_integer(destinationPID); 
        destinationPCB = get_pcb (destinationPID);
        if(destinationPCB == NULL)
        {
#ifdef DEBUG
                rtx_dbug_outs("DOOR-KERNEL : SEND MESS : DESTINATION PROCESS DOES NOT EXIST\n\r");
#endif
                return RTX_ERROR;
        }
        sourcePCB = get_pcb (sourcePID);
        enqueue_message (destinationPCB->mess_queue, (mess *) (message_envelope));

        if (destinationPCB->state == MESSAGE_WAIT)

        {
                destinationPCB->state = READY;
                enqueue_PCB_by_priority ((METAPCB_queue)READY_queue,(PCB*) destinationPCB);

                if (destinationPCB->priority > sourcePCB->priority
                    && !(in_i_process))

                {
#ifdef DEBUG
                        rtx_dbug_outs ("DOOR-KERNEL : Priority of waiting message is greater : context switching !\n\r");
#endif
                        next_pcb = dequeue_PCB_FIFO_by_priority (READY_queue);
                        context_switch ((PCB*)next_pcb);
                }
        }
        return RTX_SUCCESS;
}



void * kernel_receive_message (UINT32 * sender_ID)
{
        volatile PCB * next_pcb;

#undef DEBUG_MESSAGES
        if (in_i_process && is_msgq_empty (current_pcb->mess_queue))
        {
                msg_return = 1;
                return NULL;
        }

        else if (is_msgq_empty (current_pcb->mess_queue) && !in_i_process)

        {
                msg_return = 0;
#ifdef DEBUG_MESSAGES   
                rtx_dbug_outs("DOOR-KERNEL : MSG queue empty : going to MESSAGE_WAIT");
#endif	
                current_pcb->stack_pointer =(UINT32*) tmp_sp;
                current_pcb->state = MESSAGE_WAIT;
                next_pcb = dequeue_PCB_FIFO_by_priority (READY_queue);
                next_pcb->state = RUNNING;
                /* In case of empty queue : context switch */ 
                context_switch ((PCB*)next_pcb);
        }

        else
        {
                msg_return = 1;

#ifdef DEBUG_MESSAGES   
                rtx_dbug_outs ("\n\rReceiving message : (in pid ");
                rtx_dbug_number (current_pcb->pid);
                rtx_dbug_outs (" )\n\r");
#endif

                ret_code_recv = (UINT32) dequeueMessageFIFO (current_pcb->mess_queue);
                return (void *) ret_code_recv;
        } 
        return NULL;
}

/*Memory Management*/ 
        void *
kernel_request_memory_block (VOID)
{
		volatile PCB* next_pcb;
        memory_block mem_block;
        mem_block = dequeue_memory_block_FIFO (free_mem_q);
		
        if (mem_block == NULL)
        {
                /*I-Processes should have a pid less than 8 */ 
                if (in_i_process)
                {
                        return mem_block;
                }
                else
                {
#ifdef DEBUG
                        rtx_dbug_outs("No MORE MEMORY AVAILABLE !!\n\r");
#endif
                        current_pcb->state = MEMORY_WAIT;
                        enqueue_PCB_by_priority (MEMORY_WAIT_queue, (PCB *) current_pcb);						
                        next_pcb = dequeue_PCB_FIFO_by_priority (READY_queue);
						next_pcb->state = RUNNING;						
						context_switch ((PCB*)next_pcb);
                }                
        }
		
        return mem_block->data;
}

        int
kernel_release_memory_block (void *MemoryBlock)
{
        volatile PCB * high_prio_mem_wait_PCB;


        //enqueue the released memory block to the free memory queue
        enqueue_free_memory_queue (free_mem_q, (memory_block) MemoryBlock);

        //if the memory wait queue is not empty, dequeue the highest priority PCB
        //and enqueue to the ready queue. 
        if (!is_METAPCB_queue_empty (MEMORY_WAIT_queue))
        {
                high_prio_mem_wait_PCB = dequeue_PCB_FIFO_by_priority (MEMORY_WAIT_queue);
                high_prio_mem_wait_PCB->state = READY;
                enqueue_PCB_by_priority (READY_queue, high_prio_mem_wait_PCB);
                if (high_prio_mem_wait_PCB->priority < current_pcb->priority)
                {
#ifdef DEBUG
                     rtx_dbug_outs("\n\rWE HAVE TO CONTEXT SWITCH IN RELEASE MEMORY BLOCk\n\r");   
#endif

                     process_switch ();
                }
        }

        return RTX_SUCCESS;
}

/*Timing Service*/ 
int kernel_delayed_send(int process_ID, void * MessageEnvelope, int delay){
        mess* ptr=NULL;
        mess* prevPtr=NULL;

        ((mess*)MessageEnvelope)->dest_pid = process_ID;
        ((mess*)MessageEnvelope)->sender_pid = current_pcb->pid;

        if (delay < 0){ 
#ifdef DEBUG
                rtx_dbug_outs("Problem : negative delay\r\n");
#endif
                return RTX_ERROR;
        }

        if (delay == 0){
#ifdef DEBUG
                rtx_dbug_outs("DELAY IS 0\n\r");
#endif
                return kernel_send_message(process_ID, (mess*)MessageEnvelope);
        }

        ptr = delayed_send_queue->head;
        prevPtr = NULL;

        while (ptr != NULL && delay > ptr->delay) {
                delay = delay -  ptr->delay;
                prevPtr = ptr;
                ptr = ptr->next;
        }

        if (ptr == NULL && prevPtr == NULL)
        {
                ((mess*)MessageEnvelope)->delay = delay;
                enqueue_message(delayed_send_queue,((mess*)MessageEnvelope));
        }
        else
                //account for the case when delay is smaller than the first message envelope's delay
                if (prevPtr == NULL)
                {   
                        ptr->delay = ptr->delay - delay;
                        ((mess*)MessageEnvelope)->delay = delay;
                        ((mess*)MessageEnvelope)->next = ptr;
                        delayed_send_queue->head = (mess*)MessageEnvelope;
                }
        //account for the case when delay is larger than the last message envelope's delay
                else if (ptr == NULL)
                {
                        //       rtx_dbug_outs("case when delay is larger than the last message envelope's delay\n\r");
                        ((mess*)MessageEnvelope)->delay = delay;
                        ((mess*)MessageEnvelope)->next = NULL;
                        prevPtr->next = (mess*)MessageEnvelope;	
                }
                else
                {
                        //   rtx_dbug_outs("Between two elements\n\r");
                        ptr->delay = ptr->delay - delay;
                        ((mess*)MessageEnvelope)->delay = delay;
                        prevPtr->next = (mess*)MessageEnvelope;
                        ((mess*)MessageEnvelope)->next = ptr;
                }

        return RTX_SUCCESS;

}
/*Process Priority*/ 

int kernel_set_process_priority (UINT32 process_ID, UINT8 priority){
        PCB* pcb;
        BOOLEAN return_value;
        /*if attempting to set the priority of Null process or setting a priority
          level of 4 return error*/
        if (process_ID == 0 || priority == 4){
                return RTX_ERROR;
        }

        pcb = get_pcb(process_ID);

        /*if no process exists with the with the process ID specified return error*/
        if (pcb == NULL) {
                return RTX_ERROR;
        }

        /*if setting the priority of the current process to a lower level than the present level
          a process switch will take place*/
        if (pcb == current_pcb){
                if (priority <= pcb->priority){
                        pcb->priority = priority;
                }else{
                        pcb->priority = priority;						
                        process_switch();
                }
        }

        /*if the process is waiting for memory,the process is removed from the memory wait queue 
          and the new priority is set and enqueued back to reflect the new priority*/
        else if (pcb->state == MEMORY_WAIT){
                return_value = remove_element_by_priority(pcb,MEMORY_WAIT_queue);

                if (return_value == RTX_ERROR){
                        return RTX_ERROR;
                }

                pcb->priority = priority;
                enqueue_PCB_by_priority(MEMORY_WAIT_queue,(PCB*)pcb);
        }

        /*if the process is waiting for message, just change the priority*/
        else if (pcb->state == MESSAGE_WAIT){
                pcb->priority = priority;
        }

        /*if the process is in the ready queue and the priority level to be set is higher than the currently
          running process' priority, call process_switch()*/
        else {
				return_value = remove_element_by_priority(pcb,READY_queue);
				
                if (return_value == RTX_ERROR){
                        return RTX_ERROR;
                }

                pcb->priority = priority;
				
                enqueue_PCB_by_priority(READY_queue,(PCB*)pcb);
                				
                if (priority < current_pcb->priority){						
                        process_switch();
                }
        }

        return RTX_SUCCESS;
}




int kernel_get_process_priority (int process_ID){
        PCB* pcb = get_pcb(process_ID);

        if (pcb == NULL){
                return RTX_ERROR;
        }
        ret_code_get_prio =  pcb->priority;
        return ret_code_get_prio;
}
