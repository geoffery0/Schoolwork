#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define PAGESIZE 1024

typedef unsigned char BYTE;

unsigned char myheap[1048576];

int size1 = 1048576;



typedef struct chunkhead
{
    unsigned int size;
    unsigned int info;
    unsigned char *next, *prev;
}chunkhead;

chunkhead *head = (chunkhead*)&myheap[0];

unsigned char *mymalloc(unsigned int size){
    int pages;
    chunkhead* new;
    unsigned char* temp;
    int tempsize;
    chunkhead* hold;
    int realdemand;
    int realsize = size+sizeof(chunkhead);

    if(realsize%PAGESIZE!=0)
        pages = realsize/PAGESIZE+1;
    else
        pages = realsize/PAGESIZE;
    realdemand = pages*PAGESIZE;
    
    for(hold = head;hold!=NULL; hold = (chunkhead*)hold->next){
        if((hold->size>=realdemand) &&hold->info == 0){
            tempsize = hold->size-realdemand;
            hold->info = 1;
            if(hold->size == realdemand){
                return ((unsigned char*) ((BYTE*)hold+sizeof(chunkhead)));
            }
            hold->size = realdemand;
            new = (chunkhead*)(((BYTE*)hold) + realdemand);
            temp = hold->next;
            hold->next = (unsigned char*)new;
            new->size =tempsize-sizeof(chunkhead);
            new->info=0;
            new->prev = (unsigned char*)hold;
            new->next = temp;

            return ((unsigned char*) ((BYTE*)hold+sizeof(chunkhead)));

        }
        
    }

    return 0;

}

void myfree(unsigned char *address){
    chunkhead *chunk = ((chunkhead*)address)-1;
    if(chunk->next!=NULL && ((chunkhead*)chunk->next)->info==0){
        chunk->size += ((chunkhead*)chunk->next)->size +sizeof(chunkhead);
        chunk->next = ((chunkhead*)chunk->next)->next;
    }
    if(chunk->prev!=NULL && ((chunkhead*)chunk->prev)->info==0){
        ((chunkhead*)chunk->prev)->size = chunk->size + sizeof(chunkhead);
        ((chunkhead*)chunk->prev)->next = chunk->next;
    }
    chunk->info = 0;

}

void analyse(){
    chunkhead* chunk;
    int i = 1;
    for(chunk = head; chunk!=NULL; chunk= (chunkhead*)chunk->next){
        printf("Chunk #%d\n", i);
        printf("Size: %d\n",chunk->size);
        if(chunk->info==1)
            printf("\nOccupied\n\n");
        else
            printf("\nFree\n\n");
        printf("Next: %p\n",chunk->next);
        printf("Prev: %p\n\n\n",chunk->prev);
        i++;
    }
}