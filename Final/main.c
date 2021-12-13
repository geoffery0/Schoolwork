#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include "extract_argument.c"

void commandline(){
    printf("\n" "\033[0;34m" ">" "\033[0;37m");
    fflush(stdout);
    return;
}

int findend(char cwd[]){
    int i;
    int place;
    char slash = 47;
    for(i = 0; i<strlen(cwd);i++){
        if(cwd[i]==slash){
            place=i+1;
        }
    }
    return place;
}

void main(){
    char cwd[10000];
    char option[1000];
    char command[1000];
    char word[1000];
    char flag1[1000];
    char flag2[1000];
    getcwd(cwd,10000);
    DIR *directory = opendir(cwd);
    int hold = dup(STDOUT_FILENO);
    int file = -1;
    

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
        
        if(strcmp(command,"exit")==0){
            break;
        }

        if(strcmp(flag1,">")==0){
            file = creat(flag2,S_IRWXU|S_IRWXG|S_IRWXO|S_ISUID);
            if(file!=-1)
                dup2(file,STDOUT_FILENO);
        }
        else if(strcmp(word,">")==0){
            file = creat(flag1,S_IRWXU|S_IRWXG|S_IRWXO|S_ISUID);
            if(file!=-1)
                dup2(file,STDOUT_FILENO);
        }

        if(strcmp(command,"dir")==0){
            if(word[0]==0){
                int place = findend(cwd);
                printf("%s\n",&(cwd[place]));

                
            }
            if(strcmp(word,"/f")==0){
                
                printf("%s\n",cwd); 
            }
        }

        if(strcmp(command,"list")==0){
            directory = opendir(cwd);
            struct dirent *file = readdir(directory);
            char type[15];

            

            while(file!=NULL){
                if(file->d_type ==DT_DIR){
                    strcpy(type,"Directory");
                }else{
                    strcpy(type,"File");
                }
                printf("%s: %s\n",type,file->d_name);
                file = readdir(directory);
            }
        }

        if(strcmp(command,"cd")==0){
            directory = opendir(cwd);
            struct dirent *file = readdir(directory);

            while(file!=NULL){
                
                if(strcmp(file->d_name, word)==0 && file->d_type==DT_DIR){
                    if(strcmp(file->d_name,"..")==0){
                        cwd[findend(cwd)]=0;
                    }else if(strcmp(file->d_name,".")==0){

                    }
                    else{
                        strcat(cwd,"/");
                        strcat(cwd,file->d_name);
                    }
                    
                    directory = opendir(cwd);
                    printf("%s\n",cwd);
                    
                    
                    break;
                }
                file = readdir(directory);
            }
        }

        dup2(hold,STDOUT_FILENO);
        close(file);
    }
}