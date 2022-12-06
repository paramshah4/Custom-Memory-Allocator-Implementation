// File:	umalloc.h

// List all group members' names: Param Amit Shah (ps1169) and Vedant Gupta (vg374)
// iLab machine tested on:
//username on ilab: ps1169
//ilab server: ilab1.cs.rutgers.edu#include<stdio.h>

#include<stdio.h>
#include<stddef.h>

#define MAX_CAPACITY 1024*1024*10 // defining max capacity as roughly 10MB

#define malloc(X) umalloc(X,__FILE__,__LINE__) // macro to call umalloc
#define free(Y) ufree(Y,__FILE__,__LINE__)      // macro to call ufree


// a static char memory array 
char MEMORY[MAX_CAPACITY];
char init='0';


// struct memory_block{
//   unsigned int size;     // The size of the memory block.
//   unsigned char free;  // 1 if the block is free; 0 if the block is used.
// };

// our metadata 
struct memory_block{
  unsigned int res;
};

// default value or where the memory will start
struct memory_block *header_start = (void*)MEMORY;

// function to initialize the header in the memory
void initialize();

// function to split the memory blocks into used space and free space
void split_memory_block(struct memory_block *old_memory_block,int data_size,int old_size);

// function to merge two consecutive free memory blocks in memory
void merge_free_blocks();

// function to allocate memory to data
void *umalloc(size_t data_size,char* filename,int lineNo);

// function to free data from memory
void ufree(void* free_data,char* filename,int lineNo);
