/*--------------------------------------------------------------------------
 *
 *                      ECE354 RTX
 *
 * File name  : rtx.h
 * Description: header file defining the primitive interfaces
 *              and testing process information
 * Author     : Irene Huang i.huang@ece.uwaterloo.ca
 * Date       : June 20th, 2007
 * Note	      : setTestProcs() needs to be invoked before you can make
 *              use of RTXEVAL_TEST_PROCS[NUM_TEST_PROCS] array
 *---------------------------------------------------------------------------
 */

#ifndef _RTX_H_
#define _RTX_H_
#include "rtx_inc.h"


/* Process Priority. The bigger the number is, the lower the priority is*/
#define HIGH    0
#define MEDIUM  1
#define LOW     2
#define LOWEST  3
#define NUM_TEST_PROCS 6

#define NUM_PRIO 5
/* Primitives as specified in the project description*/
/* Interprocess Communications*/
int send_message (int process_ID, void *MessageEnvelope);
void *receive_message (int * SenderID);

/*Memory Management*/
void *request_memory_block ();
int release_memory_block (void *MemoryBlock);

/*Process Management*/
int release_processor ();

/*Timing Service*/
int delayed_send (int process_ID, void *MessageEnvelope, int delay);

/*Process Priority*/
int set_process_priority (int process_ID, int priority);
int get_process_priority (int process_ID);



/*Auto Eval Test Process Information Starts*/
#if defined (_RTX_EVAL_)
typedef struct RTXEVAL_PROC
{
  UINT32 pid;			//pid of test process
  UINT8 priority;		//priority of test process
  UINT32 stacksize;		//stack size of test process
    VOID (*rtxeval_proc_entry) ();	//entry point of test process
} RTXEVAL_PROC;

/* 
  routine that sets up RTXEVAL_TEST_PROCS[NUM_TEST_PROCS] information
  It needs to be called by your RTX process initialization routine
  before you can use RTXEVAL_TEST_PROCS[NUM_TEST_PROCS] array.
 */
extern VOID setTestProcs ();	// routine to set test proc info

/*RTXEVAL_TEST_PROCS[i] contains information of test process i*/
extern RTXEVAL_PROC RTXEVAL_TEST_PROCS[NUM_TEST_PROCS];
#undef DEBUG
#else 
#define DEBUG
#endif

/*Auto Eval Test Process Information Ends*/

#endif
