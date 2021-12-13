#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/mman.h>

void main(int argc, char *argv[]){
    int status;
    if(argc !=3){
        return;
    }
    int loops = atoi(argv[2]);
    char name[1000];



    for(int i = 0; i<loops;i++){
        int number = fork();
        
        if(number==0){
            char *args[4];
            for(int i=0; i<4; i++){
                args[i] = malloc(100);
            }
            sprintf(args[1],"%d",i);
            sprintf(args[2],"%d",loops);
            strcpy(name,argv[1]);
            strcpy(args[0],"./");
            strcat(args[0],argv[1]);
            
            args[3] = NULL;

            execv(name,args);
            return;
        }
    }

    wait(&status);
    return;
} 
