#include <rtx_inc.h>
#include <rtx_door.h>
#include <messages.h>
#include <u_processes.h>
VOID
process_b2 (VOID)
{
  mess *mymess;
  while (1)
    {
      mymess = receive_message (NULL);
      send_message (PROCESS_C2, mymess);
    }
}
