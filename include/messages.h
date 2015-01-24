#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#define MESS_DATA_SIZE 128 
typedef struct mess_t
{
  UINT32 sender_pid;
  UINT32 dest_pid;
  UINT32 type;
  UINT32 delay;
  UINT32 size;
  struct mess_t *next;
  BYTE data[MESS_DATA_SIZE];
} mess;

typedef struct message_queue_t
{
  mess *head;
  mess *tail;
} *messages_queue;

/* message queue functions */
VOID init_msg_queue (messages_queue q);
BOOLEAN is_msgq_empty (messages_queue q);
VOID enqueue_message (messages_queue messqueue, mess * message_enveloppe);
mess *dequeueMessageFIFO (messages_queue q);

mess *dequeueMessageLIFO (messages_queue q);
#endif
