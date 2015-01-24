#include <rtx_inc.h>
#include <rtx_door.h>
#include <messages.h>
#include <u_processes.h>
#include <stdio.h>
VOID
process_c2 (VOID)
{
        /* Memory initialisation for local queue */
        struct message_queue_t msg_queue_c2_s;
        messages_queue msg_queue_c2 = &msg_queue_c2_s;
        mess *p;
        mess *q;
        BOOLEAN free = FALSE;
        UINT32 num;

        init_msg_queue (msg_queue_c2);
        q = (mess *) request_memory_block ();

        while (1)
        {
                if (is_msgq_empty (msg_queue_c2))
                {
                        p = receive_message (NULL);
                }
                else
                {
                        p = dequeueMessageFIFO (msg_queue_c2);
                }
                free = TRUE;
                if (p->type == COUNT_REPORT)
                {
                        num = *(UINT32 *) (p->data);
                        if ((num % 20) == 0)
                        {
                                uart_print_string("Process C2");
                                free = FALSE;
                                q->type = WAKEUP10;
                                delayed_send (current_pcb->pid, q, 10 * DELAY_1SEC);
                                while (1)
                                {
                                        q = receive_message (NULL);
                                        if (q->type == WAKEUP10)
                                        {
                                                break;
                                        }
                                        else
                                        {
                                                enqueue_message (msg_queue_c2, q);
                                        }
                                }
                        }
                }
                if (free)
                {
                        release_memory_block (p);
                }
        }

}
