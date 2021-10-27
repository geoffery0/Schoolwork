#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define PAGESIZE 4096

typedef unsigned char BYTE;


typedef struct chunkhead
{
    unsigned int size;
    unsigned int info;
    unsigned char *next, *prev;
}chunkhead;

chunkhead *startofheap = NULL;

unsigned char *mymalloc(unsigned int size){
    int pages;
    chunkhead* new;
    chunkhead* hold;
    int realsize = size+sizeof(chunkhead);

    if(realsize%PAGESIZE!=0)
        pages = realsize/PAGESIZE+1;
    else
        pages = realsize/PAGESIZE;
    int realdemand = pages*PAGESIZE;

    if(startofheap == NULL)
    {
        startofheap = (chunkhead*)sbrk(realdemand);
        startofheap->info = 1;
        startofheap->size = realdemand-sizeof(chunkhead);
        startofheap->next = NULL;
        startofheap->prev = NULL;
        return (BYTE*)(startofheap+1);
    }

    chunkhead* best = NULL;
    
    for(hold = startofheap;hold->next; hold = (chunkhead*)hold->next){
        if((hold->size>=realdemand-sizeof(chunkhead)) && hold->info == 0){            
            if(best==NULL||best->size>hold->size)
                best = hold;
        }        
    }
    if((hold->size>=realdemand-sizeof(chunkhead)) && hold->info == 0){            
        if(best==NULL||best->size>hold->size)
            best = hold;
    }

    if(best!=NULL){
        int tempsize = best->size-realdemand;
        best->info = 1;
        if(best->size == realdemand-sizeof(chunkhead)){
            return ((BYTE*)best+sizeof(chunkhead));
        }
        best->size = realdemand-sizeof(chunkhead);
        new = (chunkhead*)(((BYTE*)best) + realdemand);
        unsigned char* temp = best->next;
        best->next = (unsigned char*)new;
        new->size =tempsize-sizeof(chunkhead);
        new->info=0;
        new->prev = (unsigned char*)best;
        new->next = temp;
        return ((BYTE*)best+sizeof(chunkhead));
    }

    new = (chunkhead*)sbrk(realdemand);
    new->info = 1;
    new->size = realdemand-sizeof(chunkhead);
    new->prev = (BYTE*)hold;
    new->next = NULL;
    hold->next = (BYTE*)new;

    return ((BYTE*)new+sizeof(chunkhead));

}

void myfree(unsigned char *address){
    chunkhead *chunk = ((chunkhead*)address)-1;

    if(chunk->prev!=NULL && ((chunkhead*)chunk->prev)->info==0){
        ((chunkhead*)chunk->prev)->size += chunk->size + sizeof(chunkhead);
        ((chunkhead*)chunk->prev)->next = chunk->next;
        if(chunk->next!=NULL)
            ((chunkhead*)chunk->next)->prev = chunk->prev;
        chunk = (chunkhead*)chunk->prev;
    }

    if(chunk->next!=NULL && ((chunkhead*)chunk->next)->info==0){
        chunk->size += ((chunkhead*)chunk->next)->size +sizeof(chunkhead);
        if(((chunkhead*)chunk->next)->next!=NULL)
            ((chunkhead*)((chunkhead*)chunk->next)->next)->prev = (BYTE*)chunk;
        chunk->next = ((chunkhead*)chunk->next)->next;
    }
    
    chunk->info = 0;

    if(chunk->next==NULL){
        
        if(chunk->prev !=NULL)
            ((chunkhead*)chunk->prev)->next = NULL;
        else
            startofheap = NULL;
        
        sbrk(-(chunk->size+sizeof(chunkhead)));
    }

}


chunkhead* get_last_chunk(){ //you can change it when you aim for performance

    if(!startofheap) //I have a global void *startofheap = NULL;
        return NULL;
    chunkhead* ch = startofheap;
    for(; ch->next; ch = (chunkhead*)ch->next);
    return ch;
}

void analyze(){
    printf("\n--------------------------------------------------------------\n");
    if(!startofheap){
        printf("no heap");
        return;
    }
    chunkhead* ch = startofheap;
    for(int no = 0; ch; ch = (chunkhead*)ch->next,no++){
        printf("%d | current addr: %p |", no, ch);
        printf("size: %d | ", ch->size);
        printf("info: %d | ", ch->info);
        printf("next: %p | ", ch->next);
        printf("prev: %p", ch->prev);
        printf("      \n");}
        printf("program break on address: %p\n",sbrk(0));
        }


void main(){


    BYTE *a[100];
    
    //clock_t ca,cb;
    //ca = clock();

    analyze();//50% points

    for(int i=0;i<100;i++)
        a[i]= mymalloc(1000);

    for(int i=0;i<90;i++)
        myfree(a[i]);
    
    analyze(); //50% of points if this is correct
    
    myfree(a[95]);
    a[95] = mymalloc(1000);

    analyze();//25% points, this new chunk should fill the smaller free one //(best fit)
    
    for(int i=90;i<100;i++)
        myfree(a[i]);
    
    //cb = clock();
    //printf("\nduration: % f\n", (double)(cb-ca));
    analyze();// 25% should be an empty heap now with the start address 
    //from the program start
    
}