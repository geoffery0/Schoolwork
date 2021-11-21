#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

//define shared memory for all int ferry[count]
//then make each pass the number of processes, what process it is, and the memoruy
//have them wait however many times you want
void synch(int count,int num, int *ferry){
    if(num=0){
        for(int i=1;i<count;i++){
            while(ferry[i]!=1){}
        }
        ferry[0] = 1;
        for(int i=1;i<count;i++){
            while(ferry[i]!=0){}
        }
        ferry[0] = 0;
        printf("\n%d\n",num);
    }
    if(num!=0){
        while(ferry[0]!=0){}
        ferry[num]=1;
        while(ferry[0]!=0){}
        ferry[num]=0;
        printf("%d",num);
    }
    
}