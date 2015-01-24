#include <rtx.h>
#include <rtx_door.h>
#include <rtx_inc.h>

#include <stdio.h>
#include <messages.h>
#include <s_processes.h>
#define one_sec 1000


VOID error_message(mess* env_to_crt, int in_prio, int in_pid)
{
        env_to_crt->data[0]='\r';
        env_to_crt->data[1]='E';
        env_to_crt->data[2]='R';
        env_to_crt->data[3]='R';
        env_to_crt->data[4]='R';
        env_to_crt->data[5]='O';
        env_to_crt->data[6]='R';
        env_to_crt->data[7]=' ';
        env_to_crt->data[8]='%';
        env_to_crt->data[9]='(';
        env_to_crt->data[10]='P';
        env_to_crt->data[11]='I';
        env_to_crt->data[12]='D';
        env_to_crt->data[13]=':';
        env_to_crt->data[14]=(CHAR)(((in_pid/10)%10)+0x30);
        env_to_crt->data[15]=(CHAR)((in_pid%10)+0x30);
        env_to_crt->data[16]=' ';
        env_to_crt->data[17]='P';
        env_to_crt->data[18]=':';
        env_to_crt->data[19]=(CHAR)((in_prio%10)+0x30);
        env_to_crt->data[20]=')';
        env_to_crt->data[21]='\r';
        env_to_crt->data[22]='\0';
        env_to_crt->size = 23;
        rtx_dbug_outs(env_to_crt->data);
        env_to_crt->type = CRT_DISPLAY_NO_PROMPT;
        send_message(CRT_PID, env_to_crt);
}




VOID set_prio_process(VOID)
{
        int in_priority=6;
        int in_process=0;
        PCB * pcb_to_change;
        mess* env_in_ptr = NULL;
        mess* env_out_ptr = NULL;

        CHAR *command=NULL;
        register_command('C');
        env_out_ptr = (mess*)request_memory_block();
        env_out_ptr->type = 0;


        while(TRUE)
        {

                env_in_ptr = (mess*)receive_message(NULL);

                rtx_dbug_outs("SET-PRIO-PROCESS : message received \n\r");
                while (env_in_ptr != NULL)
                {

                        if (env_in_ptr->sender_pid == KCD_PID && env_in_ptr->data[1] == ' ')
                        {

                                rtx_dbug_outs("SET-PRIO-PROCESS : SETTING THE  PID\n\r");
                                command = env_in_ptr->data;
                                in_process = (int)(command[2]-0x30) * 10 + (int)(command[3]-0x30); 
                                in_priority =  (int)(command[5]-0x30);
                                if ((in_process>0) && (in_process <100) && (in_priority < 4) && get_pcb(in_process))
                                {
                                        pcb_to_change =  get_pcb(in_process);
                                        rtx_dbug_outs("SET-PRIO-PROCESS : COMMAND OK\n\r");    
                                        set_process_priority(in_process,in_priority);
                                }
                                else
                                {
                                        error_message(env_out_ptr, in_priority, in_process);
                                }
                        }
                        release_memory_block(env_in_ptr);
                        env_in_ptr = (mess*)receive_message(NULL);
                }

                release_processor();

        }




}
