#include "Functions.c"

void main(){

    unsigned char *a,*b, *c;
    head->size = 1048576-sizeof(chunkhead);
    head->info = 0;
    head->next = 0;
    head->prev = 0;

    
    a = mymalloc(1000);
    b = mymalloc(1000);
    c = mymalloc(1000);
    myfree(b);
    myfree(a);
    analyse();
    printf("end");
}