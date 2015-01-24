#include <rtx_inc.h>
#include <rtx.h>
#include <rtx_door.h>
#include <task.h>
#include <stdio.h>
#include <queue.h>
#include <s_processes.h>
#include <stdio.h>
#include <string.h>
#define HOTKEYS

extern volatile METAPCB_queue READY_queue;
extern volatile METAPCB_queue MEMORY_WAIT_queue;

VOID uart_print_string(char *string)
{
mess *q;
q = (mess*) request_memory_block();
q->type=CRT_DISPLAY_NO_PROMPT;
Strncpy(q->data,string,MESS_DATA_SIZE);
send_message(CRT_PID,q);
}

VOID uart_print_integer(int num)
{
mess *q;
q = (mess*) request_memory_block();
q->type=CRT_DISPLAY_NO_PROMPT;
Strncpy(q->data,IntToStr(num),MESS_DATA_SIZE);
send_message(CRT_PID,q);
}

VOID print_pcb_infos(PCB * pcb)
{

        uart_print_string("PID ");
        uart_print_integer(pcb->pid);
        uart_print_string("  |  PRIORITY : ");
        uart_print_integer(pcb->priority);
        uart_print_string("\r");

}

VOID print_ready_queue(VOID)
{
        int i;
        PCB *seekpcb;

        for( i=0 ; i<NUM_PRIO+1 ; i++ )
        {

                /* If queue is empty */
                if( ((READY_queue->subqueues+i)->head == NULL) && ((READY_queue->subqueues+i)->tail == NULL))
                {
                        uart_print_string("QUEUE ");
                        uart_print_integer(i);
                        uart_print_string("IS EMPTY\r");
                }
                else
                {
                        seekpcb = ((READY_queue->subqueues)+i)->head;
                        while(seekpcb != ((READY_queue->subqueues)+i)->tail)
                        {
                                print_pcb_infos(seekpcb);
                                seekpcb = seekpcb->next;
                        }
                        /* Printing the last one */
                        print_pcb_infos(seekpcb);

                }

        }

}


VOID print_memory_queue(VOID)
{
        int i;
        PCB *seekpcb;

        for( i=0 ; i<NUM_PRIO+1 ; i++ )
        {

                /* If queue is empty */
                if( ((MEMORY_WAIT_queue->subqueues+i)->head == NULL) && ((MEMORY_WAIT_queue->subqueues+i)->tail == NULL))
                {
                        uart_print_string("QUEUE ");
                        uart_print_integer(i);
                        uart_print_string(" IS EMPTY\n\r");
                }
                else
                {
                        seekpcb = ((MEMORY_WAIT_queue->subqueues)+i)->head;
                        while(seekpcb != ((MEMORY_WAIT_queue->subqueues)+i)->tail)
                        {
                                print_pcb_infos(seekpcb);
                                seekpcb = seekpcb->next;
                        }
                        /* Printing the last one */
                        print_pcb_infos(seekpcb);

                }

        }

}


VOID parse_hotkey(char recv)
{

        switch(recv)
        {

                case 'Q':
                        print_ready_queue();
                        break;

                case 'M':
                        print_memory_queue();
                        break;

        }

}

VOID uart_i_process(VOID)
{

        BYTE temp;
        char recv;
        struct mess_t* env_ptr;

        /* Enable transmit interrupt. */
        //SERIAL1_IMR = 3;

        while(1)
        {
                /*
                 * Ack the interupt
                 */
                temp = SERIAL1_UCSR;

                /* See if data is waiting */    
                if(temp & 1)
                {
                        recv = SERIAL1_RD;
#ifdef HOTKEYS
                        parse_hotkey(recv);
#endif
                        env_ptr = (struct mess_t*)request_memory_block();

                        if(env_ptr != NULL)
                        {
                                env_ptr->data[0] = recv;				
                                env_ptr->data[1] = '\0';
                                env_ptr->type = KEYBOARD_INPUT;				
                                send_message(KCD_PID, env_ptr);
                        }
                        else
                        {			
                                //indicate error		
                        }

                }

                /* See if Port is ready to accept data */
                if(temp & 4)
                {			
                        /* Disable transmit interrupt. */
                        env_ptr = (mess*)receive_message(NULL);

                        while(env_ptr != NULL)
                        {

                                SERIAL1_WD = env_ptr->data[0];				
                                release_memory_block(env_ptr);				
                                env_ptr = (mess*)receive_message(NULL);
                                SERIAL1_IMR = 2;
                        }
                        /* Enable transmit interrupt. */
                }

                release_processor();
                DISABLE_INTERRUPTS();
        }

}
