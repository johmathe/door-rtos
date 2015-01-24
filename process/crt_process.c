#include <rtx_inc.h>
#include <rtx.h>
#include <stdio.h>
#include <messages.h>
#include <rtx_door.h>


VOID fancy_prompt(VOID)
{
        mess* env_out_ptr;
        env_out_ptr = (mess*) request_memory_block ();
        env_out_ptr->data[0] = 'u';
        send_message (UART_PID, env_out_ptr);
        SERIAL1_IMR = 3;

        env_out_ptr = (mess*) request_memory_block ();
        env_out_ptr->data[0] = '@';
        send_message (UART_PID, env_out_ptr);
        SERIAL1_IMR = 3;

        env_out_ptr = (mess*) request_memory_block ();
        env_out_ptr->data[0] = 'd';
        send_message (UART_PID, env_out_ptr);
        SERIAL1_IMR = 3;
        env_out_ptr = (mess*) request_memory_block ();
        env_out_ptr->data[0] = 'o';
        send_message (UART_PID, env_out_ptr);
        SERIAL1_IMR = 3;
        env_out_ptr = (mess*) request_memory_block ();
        env_out_ptr->data[0] = 'o';
        send_message (UART_PID, env_out_ptr);
        SERIAL1_IMR = 3;
        env_out_ptr = (mess*) request_memory_block ();
        env_out_ptr->data[0] = 'r';
        send_message (UART_PID, env_out_ptr);
        SERIAL1_IMR = 3;
        env_out_ptr = (mess*) request_memory_block ();
        env_out_ptr->data[0] = ':';
        send_message (UART_PID, env_out_ptr);
        SERIAL1_IMR = 3;
        env_out_ptr = (mess*) request_memory_block ();
        env_out_ptr->data[0] = '~';
        send_message (UART_PID, env_out_ptr);
        SERIAL1_IMR = 3;
        env_out_ptr = (mess*) request_memory_block ();
        env_out_ptr->data[0] = '#';
        send_message (UART_PID, env_out_ptr);
        SERIAL1_IMR = 3;
        env_out_ptr = (mess*) request_memory_block ();
        env_out_ptr->data[0] = ' ';
        send_message (UART_PID, env_out_ptr);
        SERIAL1_IMR = 3;
}

        VOID
crt_process (VOID) 
{	
        mess* env_in_ptr;
        mess* env_out_ptr;
        int i;
        while (1)    
        {
                env_in_ptr = (mess*) receive_message (NULL);

                while (env_in_ptr != NULL)	
                {

                        //if message contains a CRT display request
                        if (env_in_ptr->type == CRT_DISPLAY_REQUEST)	    
                        {
                                i=0;
                                while(env_in_ptr->data[i]!='\0' && i < MESS_DATA_SIZE)
                                {
                                        if(env_in_ptr->data[i]=='\r')
                                        {
                                                env_out_ptr = (mess*) request_memory_block ();
                                                env_out_ptr->data[0] = '\r';
                                                send_message (UART_PID, env_out_ptr);
                                                SERIAL1_IMR = 3;
                                                env_out_ptr = (mess*) request_memory_block ();
                                                env_out_ptr->data[0] = '\n';
                                                send_message (UART_PID, env_out_ptr);
                                                SERIAL1_IMR = 3;
                                                env_out_ptr = (mess*) request_memory_block ();
                                                fancy_prompt();
                                        }
                                        else
                                        {
                                                env_out_ptr = (mess*) request_memory_block ();
                                                env_out_ptr->data[0] = env_in_ptr->data[i];
                                                send_message (UART_PID, env_out_ptr);
                                                SERIAL1_IMR = 3;
                                        }
                                        i++;
                                }
                        } 
                        //Invalid message type
                        else if(env_in_ptr->type == CRT_DISPLAY_NO_PROMPT)
                        {	
                                  i=0;
                                while(env_in_ptr->data[i]!='\0' && i < MESS_DATA_SIZE)
                                {
                                        if(env_in_ptr->data[i]=='\r')
                                        {
                                                env_out_ptr = (mess*) request_memory_block ();
                                                env_out_ptr->data[0] = '\r';
                                                send_message (UART_PID, env_out_ptr);
                                                SERIAL1_IMR = 3;
                                                env_out_ptr = (mess*) request_memory_block ();
                                                env_out_ptr->data[0] = '\n';
                                                send_message (UART_PID, env_out_ptr);
                                                SERIAL1_IMR = 3;
                                                env_out_ptr = (mess*) request_memory_block ();
                                        }
                                        else
                                        {
                                                env_out_ptr = (mess*) request_memory_block ();
                                                env_out_ptr->data[0] = env_in_ptr->data[i];
                                                send_message (UART_PID, env_out_ptr);
                                                SERIAL1_IMR = 3;
                                        }
                                        i++;
                                }

                        }

                        release_memory_block (env_in_ptr);
                        env_in_ptr = (mess *) receive_message (NULL);
                }
                release_processor ();
        }
}
