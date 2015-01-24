#include <rtx_inc.h>
#include <rtx_door.h>
#include <messages.h>
#include <u_processes.h>
#include <stdio.h>

#define DEBUG_U_PROCESSES

VOID
process_a3 (VOID)
{
        UINT32 num;
        mess *p;
        p = request_memory_block ();
        register_command('Z');

        while (1)
        {
                rtx_dbug_outs ("PROCESS A3 : BEFORE RECEIVING MESSAGE\n\r");
                p = receive_message (NULL);

                if (p->data[0] == 'Z')
                {
                        rtx_dbug_outs ("PROCESS A2 : Y COMMAND, RELEASING MEMORY FOR P\n\r");
                        release_memory_block (p);
                        break;
                }
                else
                {
                        release_memory_block (p);
                }
        }

        num = 0;

        while (1)
        {

                p = request_memory_block ();
                p->type = COUNT_REPORT;
                *(UINT32 *) (p->data) = num;
                //    rtx_dbug_outs("PROCESS A : SENDING MESSAGE TO C throught B\n\r");
                send_message (PROCESS_B2, p);
                //    rtx_dbug_outs("PROCESS A : INCREMENTING NUM\n\r");
                num++;
                release_processor ();
        }

}
