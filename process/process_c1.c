#include <rtx_inc.h>
#include <rtx_door.h>
#include <messages.h>
#include <stdio.h>
#include <u_processes.h>

        VOID
process_c1 (VOID)
{
        /* Memory initialisation for local queue */
        struct message_queue_t msg_queue_c1_s;
        messages_queue msg_queue_c1 = &msg_queue_c1_s;
        mess *p;
        mess *q;
        init_msg_queue (msg_queue_c1);
        UINT32 num;


        while (1)
        {

                if (is_msgq_empty (msg_queue_c1))
                {
                        p = receive_message (NULL);
                }
                else
                {
                        p = dequeueMessageFIFO (msg_queue_c1);
                        //       rtx_dbug_outs("PROCESS C : QUEUE IS NOT EMPTY \n\r");
                }

                if (p->type == COUNT_REPORT)
                {
                        num = *(UINT32 *) (p->data);
                        rtx_dbug_outs ("\n\r");

                        if (!(num % 20))
                        {
                                rtx_dbug_outs ("PROCESS C : 20 MESSAGES RECEIVED \n\r");
                                q = (mess *) request_memory_block ();
                                uart_print_string("Process C\r");
                                p->type = WAKEUP10;
                                rtx_dbug_outs("PROCESS C : SENDING MESSAGE TO MYSELF with a delay.\n\r");
                                delayed_send (current_pcb->pid, p, 100);
                                while (1)
                                {
                                        p = receive_message (NULL);
                                        if (p->type == WAKEUP10)
                                        {
                                                rtx_dbug_outs ("PROCESS C : TYPE WAAKKKUUP 1000 !\n\r");
                                                break;

                                        }
                                        else
                                        {
                                                enqueue_message (msg_queue_c1, p);
                                        }
                                }
                        }
                        else
                        {
                                rtx_dbug_outs ("Not equal to 20\n\r");
                        }

                }
                release_memory_block (p);
                release_processor ();
        }

}
