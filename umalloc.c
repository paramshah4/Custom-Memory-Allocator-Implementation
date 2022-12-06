// File:	umalloc.c

// List all group members' names: Param Amit Shah (ps1169) and Vedant Gupta (vg374)
// iLab machine tested on:
//username on ilab: ps1169
//ilab server: ilab1.cs.rutgers.edu#include<stdio.h>


#include<stddef.h>
#include "umalloc.h"
#include <math.h>

#define METADATA_SIZE sizeof(struct memory_block) // defining the size of metadata = 4



/* Function to convert 1 byte to its decimal value */
int valuefromBits(unsigned char byte,int sum,int index)
{
     for (int i = 7; i >= 0; i--) {
        int b = byte >> i;
        if (b & 1){
            sum=sum+pow(2,(i)+(8*index));
        }
    }
    return sum;
}

/* Function to convert byte array to decimal */
int getFinalValue(struct memory_block *curr){
    int finalsum=0;
    for(int i = 1; i <4; i++){
        finalsum=valuefromBits(((char*)&curr->res)[i],finalsum,4-(i+1));
    }
    return finalsum;
}

/* Function to get free memory space in memory */
int getFreeMemorySpace(){
    int freespace=0;
    struct memory_block *curr;
    void *end;
    curr=header_start;
    end=(void*)(MEMORY + MAX_CAPACITY);
    while ((void *)curr < end ) {   // looping through memory space
        int size=getFinalValue(curr);
        if(((char*)&curr->res)[0]>>0 == '1'){ // add freespace if memory block is free
            freespace = freespace + size + METADATA_SIZE;
        }
        curr = (void *)curr + size + METADATA_SIZE;
    }
    return freespace;
}


/* Function used for debugging, prints the memory blocks metadata infromation */
void memory_dump(){
    struct memory_block *curr;
    void *end;
    curr=header_start; // start iterating from header, till we get a block where we can insert data ((first-fit algorithm))
    end=(void*)(MEMORY + MAX_CAPACITY);
    printf("-- Start of Heap (%p) --\n", curr);
    while ((void *)curr < end ) {   // While we're before the end of the heap...
        int size=getFinalValue(curr);
        printf("metadata for memory %p: (%p, size=%d, isFree=%c)\n", (void *)curr + METADATA_SIZE, curr, size, ((char*)&curr->res)[0]>>0);
        curr = (void *)curr + size + METADATA_SIZE;
    }
    printf("-- End of Heap (%p) --\n\n", end);
}

/* Function to free the memory space */
void freeMemoryOnebyOne(){
    struct memory_block *curr;
    void *end;
    curr=header_start; // start iterating from header,till the end of memory
    end=(void*)(MEMORY + MAX_CAPACITY);
    while ((void *)curr < end) {
        int size=getFinalValue(curr);  
        if ((((char*)&curr->res)[0]>>0)=='0'){ // Checking if the block is free or not, if used, set the block to free.
            ((char*)&curr->res)[0]='1';
        }
        curr = (void *)curr + size + METADATA_SIZE; // go to the next memory block.
    }
    // finalliy merging free blocks after setting all used blocks to 1.
    merge_free_blocks();
}

/* Function to initialize the header in the memory. */
void initialize(){
    int size=MAX_CAPACITY-METADATA_SIZE; // setting the block size
    ((char*)&header_start->res)[0] = '1'; // setting the block to be free
    ((char*)&header_start->res)[1] = (size >> 16) & 0xFF; // storing the block size in terms of bits.
    ((char*)&header_start->res)[2] = (size >> 8) & 0xFF;
    ((char*)&header_start->res)[3] = (size >> 0) & 0xFF;
}

/* Function to split current memory block into used and free memory blocks respectively */
void split_memory_block(struct memory_block *old_memory_block,int data_size,int curr_size){

    // creating a new memory block as space in memory block is greater than what is requried.
    struct memory_block *new_memory_block=(void*)((void*)old_memory_block + data_size + METADATA_SIZE);      
    int newsize=curr_size - data_size - METADATA_SIZE; // the size of the new block which is free
    // checking if size is enough to create a new memory block, if yes create a new memory block and set the size and its status
    // if not, just set the block to be occupied even though whole size of the block is not used.
    if(newsize > 0){  
        ((char*)&new_memory_block->res)[0] = '1';
        ((char*)&new_memory_block->res)[1] = (newsize >> 16) & 0xFF;
        ((char*)&new_memory_block->res)[2] = (newsize >> 8) & 0xFF;
        ((char*)&new_memory_block->res)[3] = (newsize >> 0) & 0xFF;
        ((char*)&old_memory_block->res)[0] = '0';
        ((char*)&old_memory_block->res)[1] = (data_size >> 16) & 0xFF;
        ((char*)&old_memory_block->res)[2] = (data_size >> 8) & 0xFF;
        ((char*)&old_memory_block->res)[3] = (data_size >> 0) & 0xFF;
    }
    else{
       ((char*)&old_memory_block->res)[0] = '0';
    }
}

/* function to merge two consecutive free memory blocks */
void merge_free_blocks(){
    struct memory_block *curr,*next; // block pointer variable to loop over the memory blocks
    void *end;
    end=(void*)(MEMORY + MAX_CAPACITY); // address where the static memory ends
    curr=header_start;
    int size=getFinalValue(curr); // getting the first block size
    int tempsize;

    // iterating over the memory space
    while((void *)curr < end && (void *)curr + size + METADATA_SIZE < end){
        next=(void *)curr + size + METADATA_SIZE;
        // check if two consecutive memory are free, if so merge them and do so consecutively.
        if(((((char*)&curr->res)[0]>>0)=='1') && ((((char*)&next->res)[0]>>0)=='1')){ 
            tempsize=getFinalValue(next); // getting the block size of next free block
            size= tempsize + METADATA_SIZE + size; // adding the next free block size to the current block size.
            // sorting the final size in the current block and deleting next free block's metadata
            ((char*)&curr->res)[1] = (size >> 16) & 0xFF; 
            ((char*)&curr->res)[2] = (size >> 8) & 0xFF;
            ((char*)&curr->res)[3] = (size >> 0) & 0xFF;
            continue;
        }
        curr=next;
    }
}

/* Function to allocate memory to data */
void *umalloc(size_t data_size,char* filename,int lineNo){
    
    // Checking if 0 or less than 0 Byte data is to be inserted. Raise an error if try to do so.
    if(data_size<=0){ 
        printf("Error in file %s line %d : The amount requested makes no sense \n",filename,lineNo);
        return NULL;
    }
    // Inilializing block pointer to store the memory block where data can be inserted 
    struct memory_block *curr;
    void *end;
    void *result; // variable to return the pointer to where the data is inserted
    if(init=='0'){ // we check whether the  memory has been intialized or not
        initialize(); 
        init='1';
    }
    curr=header_start; // start iterating from header, till we get a block where we can insert data ((first-fit algorithm))
    end=(void*)(MEMORY + MAX_CAPACITY);
    int size=getFinalValue(curr);
    // loop to get the block where we can insert this requested data.
    while (((void *)curr + size + METADATA_SIZE < end)&&(((((char*)&curr->res)[0]>>0)=='0')||size < data_size + METADATA_SIZE)) {   // While we're before the end of the heap...
        curr = (void *)curr + size + METADATA_SIZE;
        size=getFinalValue(curr);
    }
    // checking if size of memory block is equal to the data requested to insert.
    if(size==data_size && ((((char*)&curr->res)[0]>>0)=='1')){
        ((char*)&curr->res)[0] = '0';
        result=(void *)curr + METADATA_SIZE;
        return result;
    }
    // checking if size of memory block is greater than what is requested to insert.
    // if so we split memory blocks into used and free memory blocks.
    else if(size > data_size && ((((char*)&curr->res)[0]>>0)=='1')){ 
        split_memory_block(curr,data_size,size);
        result=(void *)curr + METADATA_SIZE;
        return result;
    }
    else{
        result=NULL;
        int totalfreespace=getFreeMemorySpace();
        // handling the different error scenarios using the total free space left in memory.
        if(totalfreespace!=0){
            if(totalfreespace > data_size + METADATA_SIZE){
                printf("Error in file %s line %d : There is enough free memory, but there is no block large enough for the allocation \n",filename,lineNo);
            }
            else{
            printf("Error in file %s line %d : Sorry. memory is not full but there is not enough free memory for the allocation \n",filename,lineNo);    
            }
        }
        else{
            printf("Error in file %s line %d : Sorry.There is no free memory \n",filename,lineNo);    
        }
        return result;
    }
}

/* Function to de-allocate the memory space given pointer to address */
void ufree(void* free_data,char* filename,int lineNo){
 // we check if its a valid pointer to address , if not throw error   
    if(((void*)MEMORY <= free_data) && (free_data <= (void*)(MEMORY + MAX_CAPACITY))){
    struct memory_block* curr = free_data - METADATA_SIZE;
    // checking if the memory block is already freed.
    if((((char*)&curr->res)[0]>>0)=='1'){ 
        printf("Error in file %s line %d : Redundant Pointer Freeing \n",filename,lineNo);
        return;
    }
    // checking if the memory block is allocated by the umalloc or not
    else if(!((((char*)&curr->res)[0]>>0)=='0' || (((char*)&curr->res)[0]>>0)=='1')){ 
        printf("Error in file %s line %d : Freeing Pointer not allocated by malloc \n",filename,lineNo);
        return;
    }
    int size=getFinalValue(curr);
    // totalFreeSize+=size; // incrementing the free size
    ((char*)&curr->res)[0]='1';  // setting the memory block free
    merge_free_blocks();  // merge to join two free memory blocks if any
    }
    else {
        printf("Error in file %s line %d : Freeing addresses not present in memory \n",filename,lineNo);
    }
}
