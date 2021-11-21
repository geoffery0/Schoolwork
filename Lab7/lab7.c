#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define byte unsigned char

typedef struct mypipe{
    byte* pipebuffer;
    int buffersize;
    int start_occupied;
    int end_occupied;
    }mypipe;
    
    //initializes (malloc) the pipe with a size of "size" and sets start and end.
    void init_pipe(mypipe* pipe, int size){
        pipe->buffersize = size;
        pipe->start_occupied = 0;
        pipe->end_occupied = 0;
        pipe->pipebuffer = malloc(size);
    }
    
    //writes "size" bytes from buffer into thepipe, returns size
    int mywrite(mypipe *pipe, byte* buffer, int size){
        int i;
        for(i=0;i<size;i++){
            if(pipe->end_occupied%(pipe->buffersize-1)!=pipe->start_occupied-1){
                pipe->pipebuffer[pipe->end_occupied++] = buffer[i];
                if(pipe->end_occupied == pipe->buffersize)
                    pipe->end_occupied = 0;
            }
            else{
                break;
            }
        }
        return size;
    }

    //reads "size" bytes from pipe into buffer, returns how much it read (max size), 0 if pipe is empty
    int myread(mypipe* pipe, byte* buffer, int size){
        int i;
        int count=0;
        for(i=0;i<size;i++){
            if(pipe->start_occupied==pipe->end_occupied){
                return count;
            }
            buffer[i] = pipe->pipebuffer[pipe->start_occupied++];
            if(pipe->start_occupied == pipe->buffersize)
                pipe->start_occupied = 0;
            count++;
        }
        return count;

    }

void main(){

    char text[100];
    mypipe pipeA;
    init_pipe(&pipeA, 32);
    mywrite(&pipeA, "hello world", 12);
    mywrite(&pipeA, "it's a nice day", 16);
    myread(&pipeA, text, 12);
    printf("%s\n", text);
    myread(&pipeA, text, 16);
    printf("%s\n", text);
    mywrite(&pipeA, "and now we test the carryover", 30);
    myread(&pipeA, text, 30);
    printf("%s\n", text);
}