#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/mman.h>
#include "extract_argument.c"

struct children{
    int pid;
    char doing[1000];
    char current;
};

int fd[2];
int name[2];

void mysighandler(int i){
    int count;
    pid_t child;
    size_t size;
    int status;
    char file[10000];
    read(name[0],&count,sizeof(int));
    read(name[0],&child,sizeof(pid_t));

    if(count ==0){
        printf("nothing found\n");
        fflush(stdout);
    }else{
        for(int i=0;i<count;i++){
            read(fd[0],&size,sizeof(size_t));
            read(fd[0],file,size);
            file[size] = 0;
            printf("found: %s\n",file);
            fflush(stdout);
        }
    }
    kill(child,SIGKILL);
    waitpid(child,&status,0);
}

void findfile(char word[],int s,char cwd[],int fd[],int *count,char *prev){
    DIR *directory = opendir(cwd);
    char print[10000];
    char current[10000];
    struct dirent *file = readdir(directory);

    while(file!=NULL){
        
        if(strcmp(file->d_name,word)==0){
            //printf("found it");
            sprintf(print,"%s%s",prev, file->d_name);
            unsigned long length = strlen(print);
            write(fd[1],&length,sizeof(size_t));
            write(fd[1],print,strlen(print));
            //printf("%u",(unsigned int)strlen(print));
            
            //printf("count: %d\n",*count);
            //fflush(stdout);
            *count+=1;   
             
        }
        if((s==1 && file->d_type==DT_DIR)&&!(strcmp(file->d_name,".")==0 || strcmp(file->d_name,"..")==0)){
            //printf("%s\n",file->d_name);
            
            strcpy(current,prev);
            strcat(current,file->d_name);
            strcat(current,"/");
            
            findfile(word,s,file->d_name,fd,count,current);
            
        }
        file = readdir(directory);
    }
}

void commandline(){
    printf("\033[0;34m" "findstuff" "\033[0;37m" "$ ");
    fflush(stdout);
    return;
}

void main(){
    char option[1000];
    char command[1000];
    char word[1000];
    char flag1[1000];
    char flag2[1000];
    char *temp;

    int parent = getpid();
    
    clock_t start,end;

    signal(SIGHUP,mysighandler);

    pipe(fd);
    pipe(name);

    struct children *children = mmap(NULL,sizeof(struct children)*10,PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON,-1,0);

    for(int i = 0; i<10; i++){
        children[i].current = 0;
    }
    while(1){
        commandline();
        int r = read(STDIN_FILENO,option,1000);
        option[r-1] = 0;
        if(!get_argument(option,0,command))
            command[0] = 0;
        if(!get_argument(option,1,word))
            word[0] = 0;
        if(!get_argument(option,2,flag1))
            flag1[0] = 0;
        if(!get_argument(option,3,flag2))
            flag2[0]=0;
        
        if(strcmp("find",command)==0){
            int found = 0;
            for(int i=0; i<10; i++){
                if(children[i].current==0){
                    children[i].current = 1;
                    
                    if(fork()==0){
                        int count = 0;
                        close(fd[0]);
                        close(name[0]);
                        //close(fd[1]);
                        start = clock();
                        
                        

                        children[i].pid=getpid();
                        if(word[0]!=0){
                            if(word[0]=='"'){
                                strcpy(children[i].doing,"finding a word in files");
                                while(1);
                            }

                            else{
                                int s = 0;
                                char cwd[10000];
                                getcwd(cwd,10000);
                                strcpy(children[i].doing,"finding a file in the directory");
                                if(strcmp(flag1,"-s")==0 || strcmp(flag2,"-s")==0){
                                    s=1;
                                    
                                    
                                }
                                
                                findfile(word,s,cwd,fd,&count,"");
                                //printf("%d",count);
                                //fflush(stdout);
                                write(name[1],&count,sizeof(int));
                                write(name[1],&(children[i].pid),sizeof(pid_t));
                                kill(parent,SIGHUP);
                            }
                        }
                        end = clock();

                        children[i].current=0;
                        return;
                    }
                    found = 1;
                    break;
                }
            }
            if(!found){
                printf("\nsorry to tell you but too many children are alive right now to birth another!\n");
            }
        }

        if(strcmp("quit",command)==0 || strcmp("q",command)==0){
            for(int i = 0; i<10; i++){
                if(children[i].current)
                    kill(children[i].pid,SIGINT);
            }
            munmap(children,sizeof(struct children)*10);
            close(fd[0]);
            close(fd[1]);
            close(name[0]);
            close(name[1]);
            wait(0);
            return;
        }
        
        if(strcmp("kill",command)==0){
            int pid = atoi(word);
            for(int i = 0; i<10; i++){
                if(children[i].pid==pid){
                    children[i].current=0;
                    kill(pid,SIGINT);
                    break;
                }
            }
        }

        if(strcmp("list",command)==0){
            for(int i = 0; i<10; i++){
                if(children[i].current){
                    printf("pid: %d\nactivity: %s\n",children[i].pid,children[i].doing);
                }
            }
        }
    }
}