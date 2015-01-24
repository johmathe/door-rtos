#include <rtx.h>
#include <rtx_inc.h>
#include <rtx_door.h>
#include <task.h>
#include <stdio.h>
#include <u_processes.h>
#include <s_processes.h>

extern volatile task_init tasks[6];
extern UINT32 kernel_clock;


VOID process1 (VOID);
VOID process2 (VOID);
VOID process3 (VOID);
VOID process4 (VOID);


PCB *
get_pcb (UINT32 pid)
{
        UINT32 i;
        for (i = 0; i < MAX_PROCESSES; i++)
        {
                if (pcbs[i].pid == pid)
                {
                        return (PCB *) (pcbs + i);
                }
        }
        /* If we didn't find anything */
        rtx_dbug_outs("get PCB DIDNT FIND ANY PROCESS\n\r");
        rtx_print_integer(pid);
        return NULL;
}

VOID
init_tasks (VOID)
{
        int i;
#ifdef DEBUG
        rtx_dbug_outs ("Booting : In the init_tasks func\n\r");
#endif
        for (i = 1; i < 4; i++)
        {
                tasks[i].pid = i;
                tasks[i].priority = MEDIUM;
                tasks[i].stacksize = DEFAULT_STACK_SIZE;
        }


        tasks[1].rtxeval_proc_entry = process2;
        tasks[2].rtxeval_proc_entry = process3;
        tasks[3].rtxeval_proc_entry = process4;


#ifdef DEBUG
        rtx_dbug_outs ("Booting : Tasks are initialized\n\r");
#endif

}


VOID
process1 (VOID)
{
        while (1)
        {
                release_processor ();
        }

}

VOID
process2 (VOID)
{
        while (1)
        {
                release_processor ();
        }
}


VOID
process3 (VOID)
{
        while (1)
        {
                release_processor();
        }
}

VOID
process4 (VOID)
{
        while (1)
        {
                release_processor();
        }
}
