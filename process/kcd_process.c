#include <rtx_inc.h>
#include <rtx_door.h>
#include <rtx.h>
#include <u_processes.h>
#include <s_processes.h>
#include <stdio.h>
#include <string.h>

UINT32 register_command(CHAR command)
{
        mess * p;
        p = (mess*) request_memory_block();
        p->type = COMMAND_REGISTRATION;
        p->sender_pid = current_pcb->pid;
        p->data[0] = command;
        return(send_message(KCD_PID, p));
}


VOID kcd_process (VOID)
{

        /* Mapping between command and processes */
        UINT32 map[128];
        char* errmess = "\rnot OK";
        int i=0;
        int char_typed = 0;
        char c;
        BOOLEAN buffer_flag =0;
        mess *kcd_mess=NULL;
        mess *tosend=NULL;
        mess *error=NULL;
        while (1)
        {
                kcd_mess = receive_message (NULL);
                if (kcd_mess->type == COMMAND_REGISTRATION)
                {
                        rtx_dbug_outs ("KCD-PROCESS : reg\n\r");
                        kcd_mess->data[1] = '\0';
                        rtx_dbug_outs(kcd_mess->data);
                        rtx_print_integer(kcd_mess->sender_pid);
                        map[kcd_mess->data[0]] = kcd_mess->sender_pid;
                }
                else if (kcd_mess->type == KEYBOARD_INPUT)
                {
                        c = kcd_mess->data[0];

                        if (c == '%' && !buffer_flag)
                        {
                                tosend = (mess *) request_memory_block ();
                                buffer_flag = TRUE;
                                rtx_dbug_outs ("KCD-PROCESS : COMMAND DETECTED\n\r");
                        }
                        else  if (buffer_flag)
                        {
                                if (c == '\r')
                                {
                                        i=0;
                                        rtx_dbug_outs ("KCD-PROCESS : SENDING MESSAGE\n\r");
                                        send_message (map[tosend->data[0]], tosend);
                                        buffer_flag = 0;
                                        
                                }
                                else if (i< MAX_COMMAND_SIZE)
                                {
                                        tosend->data[i] = c; 
                                        i++;
                                }
                        }
                        else if (char_typed > 2 && (c == '\r'))
                        {
                            error = (mess *) request_memory_block ();
                            Strncpy(error->data, errmess,32);
                            error->type = CRT_DISPLAY_NO_PROMPT;
                            send_message (CRT_PID, error);   
                            char_typed = 0;

                        }
                        else if (c == '\r')
                        {
                            char_typed = 0;
                        }
                        else 
                        {
                            char_typed++;
                        }
                }
                /* Local echo */
                kcd_mess->type = CRT_DISPLAY_REQUEST;
                send_message (CRT_PID, kcd_mess);
        }

}
