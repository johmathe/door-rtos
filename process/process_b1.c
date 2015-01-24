#include <rtx_inc.h>
#include <rtx_door.h>
#include <messages.h>
#include <u_processes.h>
#include <stdio.h>

VOID
process_b1 (VOID)
{
  mess *mymess;
  while (1)
    {
      mymess = receive_message (NULL);
      send_message (PROCESS_C1, mymess);
    }
}
