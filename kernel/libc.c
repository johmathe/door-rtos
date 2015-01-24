#include <rtx_door.h>
#include <messages.h>


VOID request_delay(messageEnvelope* env_to_timer)
{
  env_to_timer->type = INT;
  env_to_timer->data = one_sec;
  send_message(PID_TIMER_PROCESS, env_to_timer);
  receive_message();
}

