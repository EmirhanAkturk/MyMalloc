#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
typedef enum {bestFit, worstFit, firstFit} Strategy;

Strategy strategy = bestFit;
typedef struct{
    unsigned int size;
    unsigned int isfree;
}Info;

typedef struct{
  Info info;           /*number of  16 byte blocks*/
  struct Block *next;  /*next free*/
  char data[0];        /*start of the allocated memory*/
 }Block;
/*Block: |4byte(size)|4byte(isfree)|8-byte next|0-byte(data)|*/

static Block *free_list = NULL;     /*start of the free list*/
static Block *heap_start = NULL;    /*head of allocated memory from sbrk */
static Block *heap_end = NULL; 


void *mymalloc(size_t size);
void *myfree(void *p);
Block *split(Block *b, size_t size);
int test(int fitStrategy);
Block* BestFit(const unsigned int size);
Block* WorstFit(const unsigned int size);
Block* FirstFit(const unsigned int size);
void printFreeList();
