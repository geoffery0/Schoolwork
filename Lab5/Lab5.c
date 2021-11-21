#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>

void mysighandler(int i){
    printf("hah nope");
}

void main(){
    char cwd[256];
    struct dirent *file;
    int a;

    pid_t child = 0;
    pid_t c;
    pid_t p;
    time_t T= time(NULL);
    //struct tm tm = *localtime(&T);

    signal(SIGINT,mysighandler);
    signal(SIGABRT,mysighandler);
    signal(SIGHUP,mysighandler);
    signal(SIGQUIT,mysighandler);
    signal(SIGILL,mysighandler);
    signal(SIGTRAP,mysighandler);
    signal(SIGBUS,mysighandler);
    signal(SIGFPE,mysighandler);
    signal(SIGUSR1,mysighandler);
    signal(SIGSEGV,mysighandler);
    signal(SIGUSR2,mysighandler);
    signal(SIGPIPE,mysighandler);
    signal(SIGALRM,mysighandler);
    signal(SIGTERM,mysighandler);
    signal(SIGSTKFLT,mysighandler);
    signal(SIGCHLD,mysighandler);
    signal(SIGCONT,mysighandler);
    signal(SIGSTOP,mysighandler);
    signal(SIGKILL,mysighandler);
    signal(SIGTSTP,mysighandler);
    signal(SIGTTIN,mysighandler);
    signal(SIGTTOU,mysighandler);
    signal(SIGURG,mysighandler);
    signal(SIGXCPU,mysighandler);
    signal(SIGXFSZ,mysighandler);
    signal(SIGVTALRM,mysighandler);
    signal(SIGPROF,mysighandler);
    signal(SIGWINCH,mysighandler);
    signal(SIGIO,mysighandler);
    signal(SIGPWR,mysighandler);
    signal(SIGSYS,mysighandler);
    signal(SIGRTMIN,mysighandler);
    signal(SIGRTMIN+1,mysighandler);
    signal(SIGRTMIN+2,mysighandler);
    signal(SIGRTMIN+3,mysighandler);
    signal(SIGRTMIN+4,mysighandler);
    signal(SIGRTMIN+5,mysighandler);
    signal(SIGRTMIN+6,mysighandler);
    signal(SIGRTMIN+7,mysighandler);
    signal(SIGRTMIN+8,mysighandler);
    signal(SIGRTMIN+9,mysighandler);
    signal(SIGRTMIN+10,mysighandler);
    signal(SIGRTMIN+11,mysighandler);
    signal(SIGRTMIN+12,mysighandler);
    signal(SIGRTMIN+13,mysighandler);
    signal(SIGRTMIN+14,mysighandler);
    signal(SIGRTMIN+15,mysighandler);
    signal(SIGRTMAX,mysighandler);
    signal(SIGRTMAX-1,mysighandler);
    signal(SIGRTMAX-2,mysighandler);
    signal(SIGRTMAX-3,mysighandler);
    signal(SIGRTMAX-4,mysighandler);
    signal(SIGRTMAX-5,mysighandler);
    signal(SIGRTMAX-6,mysighandler);
    signal(SIGRTMAX-7,mysighandler);
    signal(SIGRTMAX-8,mysighandler);
    signal(SIGRTMAX-9,mysighandler);
    signal(SIGRTMAX-10,mysighandler);
    signal(SIGRTMAX-11,mysighandler);
    signal(SIGRTMAX-12,mysighandler);
    signal(SIGRTMAX-13,mysighandler);
    signal(SIGRTMAX-14,mysighandler);

    getcwd(cwd,256);
    p = getpid();
    printf("Parent: %d\n", p);


    while(child==0 || waitpid(child,&a,0)){
        child = fork();
        if(child==0){
            c = getpid();
            
            time_t T = time(NULL);
            struct tm tm = *localtime(&T);
            
            
            while(1){
                printf("Child: %d\n",c);
                printf("Time: %d:%d\n",tm.tm_hour,tm.tm_min);
                DIR *directory = opendir(cwd);
                file = readdir(directory);
                while(file!=NULL){
                    printf("%s\n", file->d_name);
                    file = readdir(directory);
                }
                sleep(10);

            }
            return;
        }
    }

}