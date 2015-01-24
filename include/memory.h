#ifndef __MEMORY_H__
#define __MEMORY_H__
#define NUM_MEM_BLOCKS 256 
#define MEM_BLOCK_SIZE 256 

typedef struct memory_block_t
{
  BYTE data[MEM_BLOCK_SIZE];
  struct memory_block_t *next;
} *memory_block;

typedef struct free_memory_queue_t
{
  memory_block head;
  memory_block tail;
} *free_memory_queue;

extern free_memory_queue free_mem_q;


#endif
