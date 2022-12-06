#include<stdio.h>
#include "umalloc.c"
#include <time.h>
#include <math.h>
#define BLOCK_SIZE 1
#define ONE_MB_BLOCK_SIZE 1024*1024
#define ONE_KB_BLOCK_SIZE 1024


void consistency(){
    printf("TESTING CONSISTENCY \n");
    int *k=(int *)malloc(BLOCK_SIZE);
    printf("Address of 1B allocated block %p \n",&(*k));
    printf("Freeing the block \n");
    free(k);
    int *p=(int *)malloc(BLOCK_SIZE);
    printf("Address of again allocating 1B block %p \n",&(*p));
    free(p);
     printf("SUCCESS CONSISTENCY \n");
    return;
}
unsigned long maximization(){
    printf("TESTING MAXIMIZATION \n");
   int flag=1;
   int mult=1; 
   int max_alloc=0;
    while(flag){
     int *k=(int*)malloc(mult*BLOCK_SIZE);
     if(k==NULL){
        max_alloc=mult/2;
        flag=0;
        break;
     }
     else{
       free(k); 
     }
     mult*=2;
}
printf("Maximum Allocation Size : %d \n",max_alloc*BLOCK_SIZE);
printf("SUCCESS MAXIMIZATION \n");
return max_alloc*BLOCK_SIZE;
}

void basic_coalescence(unsigned long max_alloc){
    printf("TESTING BAISC COALESCENCE \n");
    int *temp=(int *)malloc(max_alloc/2);
    printf("Allocated 1/2 of maximum allocation \n");
    int *temp2=(int *)malloc(max_alloc/4);
     printf("Allocated 1/4th of maximum allocation \n");
    free(temp);
    free(temp2);
     printf("Successfully freed both data \n");
    int *temp3=(int *)malloc(max_alloc);
    printf("Successfully added maximum allcoation \n");
    free(temp3);
    printf("SUCCESS BAISC COALESCENCE \n");
}

void saturation(){
    printf("TESTING SATURATION \n");
    for (int i=0;i<9000;i++){
        int *k=(int *)malloc(ONE_KB_BLOCK_SIZE);
    }
    int cnt=0;
    int flag=1;
     int *prev;
    while(flag){
     int *p=(int *)malloc(BLOCK_SIZE);
     if(p==NULL){
        flag=0;
     }
     else{
        cnt+=1;
        prev=p;
     }   
    }
printf("Maximal number of allocations: %d \n",cnt);
printf("Freeing last block for time overhead test \n");
free(prev);
printf("SUCCESS SATURATION \n");
}

void time_overhead(){
    printf("TESTING TIME OVERHEAD \n");
    clock_t t;
    t = clock();
    int *r=(int *)malloc(BLOCK_SIZE);
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; 
    printf("Max Time Overhead :  %f seconds \n", time_taken);
}
void intermediate_coalescence(unsigned long max_alloc){
    printf("TESTING INTERMEDIATE COALESCENCE \n");
    freeMemoryOnebyOne();
    printf("Freed the memory. Now making an attempt to allocate maximum allocaltion \n");
    int *temp=(int *)malloc(max_alloc);
    printf("Allocated maximum allocation. Success! \n");
    free(temp);
    printf("SUCCESS INTERMEDIATE COALESCENCE \n");
}


void mallocTests(){
    printf("MALLOC ERROR TEST STARTS \n");
    printf("*******************************************************\n");
    int *k=(int *)malloc(0);
    printf("------------------------ \n");
    int *p=(int *)malloc(9*ONE_MB_BLOCK_SIZE);
    int *p1=(int *)malloc(ONE_MB_BLOCK_SIZE);
    free(p);
    printf("------------------------ \n");
    int * a1=(int *)malloc(2*ONE_MB_BLOCK_SIZE);
    int * a2=(int *)malloc(2*ONE_MB_BLOCK_SIZE);
    int * a3=(int *)malloc(2*ONE_MB_BLOCK_SIZE);
    int * a4=(int *)malloc(2*ONE_MB_BLOCK_SIZE);
    free(a2);
    free(a4);
    int * a5=(int *)malloc(4*ONE_MB_BLOCK_SIZE);
    free(a1);
    free(a3);
    printf("------------------------ \n");
    int * b1=(int *)malloc(10485750*BLOCK_SIZE);
    int * b2=(int *)malloc(2*BLOCK_SIZE);
    int * b3=(int *)malloc(1*BLOCK_SIZE);
    free(b1);
    free(b2);
     printf("------------------------ \n");
}

void ufreeTest(){
     printf("UFREE ERROR TEST STARTS \n");
     printf("*******************************************************\n");
    int *k=(int *)malloc(BLOCK_SIZE);
    free(k+4);
    printf("------------------------ \n");
    int *p=(int *)malloc(BLOCK_SIZE);
    free(p);
    free(p);
    printf("------------------------ \n");
    int x;
    free((int *)x);
     printf("------------------------ \n");
}

int print(unsigned char byte,int sum,int index)
{
     for (int i = 7; i >= 0; i--) {
        int b = byte >> i;
        if (b & 1){
            // printf("1");
            sum=sum+pow(2,(i)+(8*index));
        }
        else{
            //  printf("0");
        }
    }
    return sum;
}
int main(){
    printf("*******************************************************\n");
    printf("BASIC INTEGRATION TEST STARTS \n");
    printf("*******************************************************\n");
    consistency();
    printf("*******************************************************\n");
    unsigned long maximum_allocation=maximization();
    printf("*******************************************************\n");
    basic_coalescence(maximum_allocation);
    printf("*******************************************************\n");
    saturation();
    printf("*******************************************************\n");
    time_overhead();
    printf("*******************************************************\n");
    intermediate_coalescence(maximum_allocation);
    printf("*******************************************************\n");
    printf("BASIC INTEGRATION TEST ENDS \n");
    printf("*******************************************************\n");


    printf("*******************************************************\n");
    printf("ERROR DETECTION TEST STARTS \n");
    printf("*******************************************************\n");
    mallocTests();
    printf("*******************************************************\n");
    ufreeTest();
    printf("*******************************************************\n");
    printf("ERROR DETECTION TEST ENDS \n");
    printf("*******************************************************\n");

    printf("Size of Metadata: %lu\n",METADATA_SIZE);

}
