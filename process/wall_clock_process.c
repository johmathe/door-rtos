#include <rtx.h>
#include <rtx_door.h>
#include <rtx_inc.h>

#include <stdio.h>
#include <messages.h>
#include <s_processes.h>
#define one_sec 1000


VOID output_clock(mess* env_to_crt, int out_hour, int out_min, int out_sec)
{
  env_to_crt->data[0]='\r';
  env_to_crt->data[1]=(CHAR)(((out_hour/10)%10)+0x30);
  env_to_crt->data[2]=(CHAR)((out_hour%10)+0x30);
  env_to_crt->data[3]=':';
  env_to_crt->data[4]=(CHAR)(((out_min/10)%10)+0x30);
  env_to_crt->data[5]=(CHAR)((out_min%10)+0x30);
  env_to_crt->data[6]=':';
  env_to_crt->data[7]=(CHAR)(((out_sec/10)%10)+0x30);
  env_to_crt->data[8]=(CHAR)((out_sec%10)+0x30);
  env_to_crt->data[9]='\r';
  env_to_crt->data[10]='\0';
  env_to_crt->size = 11;
  rtx_dbug_outs(env_to_crt->data);
  env_to_crt->type = CRT_DISPLAY_NO_PROMPT;
  send_message(CRT_PID, env_to_crt);
}


VOID wall_clock_process(VOID)
{

  int hour=0, min=0, sec=0;
  CHAR* in_time;
  int in_hour, in_min, in_sec;
  BOOLEAN wall_clock_on = FALSE;
  
  mess* env_in_ptr = NULL;
  mess* env_out_ptr = NULL;

  register_command('W');
  env_out_ptr = (mess*)request_memory_block();
  env_out_ptr->type = 0; 
  
  while(TRUE)
  {
  
    env_in_ptr = (mess*)receive_message(NULL);

//    rtx_dbug_outs("WALL-CLOCK : message received \n\r");
    while (env_in_ptr != NULL)
    {
 //   rtx_dbug_outs("WALL-CLOCK : message received \n\r");
    
      if (env_in_ptr->sender_pid == KCD_PID && env_in_ptr->data[1] == 'S')
      {
            
        rtx_dbug_outs("WALL-CLOCK-PROCESS : SETTING THE CLOCK \n\r");
        //Note: this currently assumes the input is correct
        //todo: add error checking, if invalid command, do nothing
        //Set time
        in_time = env_in_ptr->data;
    
        in_hour = (int)(in_time[2]-0x30) * 10 + (int)(in_time[3]-0x30);
        in_min = (int)(in_time[5]-0x30) * 10 + (int)(in_time[6]-0x30);
        in_sec = (int)(in_time[8]-0x30) * 10 + (int)(in_time[9]-0x30);
        
        if ((in_hour >= 0 && in_hour < 24) && (in_min >= 0 && in_min < 60) && (in_sec >= 0 && in_min < 60))
        {
      
         rtx_dbug_outs("WALL-CLOCK-PROCESS : TIME IS A GOOD FORMAT \n\r");
         hour = in_hour;
         min = in_min;
         sec = in_sec;
          //Output time
          env_out_ptr = (mess*)request_memory_block();
          env_out_ptr->type = 0;
     
         output_clock(env_out_ptr, hour, min, sec);
           rtx_dbug_outs("WALL-CLOCK-PROCESS : OUTPUT OK : requesting memory \n\r");
          //Send delay request to timer i-process
     
          env_out_ptr = (mess*)request_memory_block();
          env_out_ptr->type = 0;
          rtx_dbug_outs("WALL-CLOCK-PROCESS : SENDING DELAY  \n\r");
          wall_clock_on = 1;
          delayed_send(current_pcb->pid, env_out_ptr ,100);
        }
      
       }
      else if(env_in_ptr->sender_pid == KCD_PID  && env_in_ptr->data[1] == 'C')
      {

        rtx_dbug_outs("WALLL CLOCK PROCESS : GETTING CLOCK !!!!!\n\r");
        env_out_ptr = (mess*)request_memory_block();
        output_clock(env_out_ptr, hour, min, sec);
 

      }
      else if (env_in_ptr->sender_pid == KCD_PID && env_in_ptr->data[1] == 'T')
      {
        rtx_dbug_outs("WALLL CLOCK PROCESS : DESACTIVATING WALL CLOCK\n\r");
        wall_clock_on = FALSE;
      }
      else if (env_in_ptr->sender_pid == TIMER_PID && wall_clock_on)
      {
        //increment time
        sec++;
        if (sec >= 60)
        {
          sec = 0;
          min++;
          if (min >= 60)
          {
            min = 0;
            hour++;
            if (hour >= 24)
            {
              hour = 0;
            }
          }
        }

        env_out_ptr = (mess*)request_memory_block();
        output_clock(env_out_ptr, hour, min, sec);

        //Output time
        env_out_ptr = (mess*)request_memory_block();
        env_out_ptr->type = 0;
        delayed_send(current_pcb->pid, env_out_ptr ,100);

      }
      release_memory_block(env_in_ptr);
      
      env_in_ptr = (mess*)receive_message(NULL);
    }
    
    release_processor();
  
  }




}
