#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

void main(){
    if(fork()==0){
        printf("Justin ");
        return;
    }
    wait(0);
    if(fork()==0){
        printf("Paul ");
        return;
    }
    wait(0);
    printf("Scabarozi");
}