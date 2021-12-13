#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MATRIX_DIMENSION_XY 10



//SEARCH FOR TODO


//************************************************************************************************************************
// sets one element of the matrix
void set_matrix_elem(float *M,int x,int y,float f)
{
M[x + y*MATRIX_DIMENSION_XY] = f;
}
//************************************************************************************************************************
// lets see it both are the same
int quadratic_matrix_compare(float *A,float *B)
{
for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
       if(A[a +b * MATRIX_DIMENSION_XY]!=B[a +b * MATRIX_DIMENSION_XY]) 
        return 0;
   
return 1;
}
//************************************************************************************************************************
//print a matrix
void quadratic_matrix_print(float *C)
{
    printf("\n");
for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
    {
    printf("\n");
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
        printf("%.2f,",C[a + b* MATRIX_DIMENSION_XY]);
    }
printf("\n");
}
//************************************************************************************************************************
// multiply two matrices
void quadratic_matrix_multiplication(float *A,float *B,float *C)
{
	//nullify the result matrix first
for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
        C[a + b*MATRIX_DIMENSION_XY] = 0.0;
//multiply
for(int a = 0;a<MATRIX_DIMENSION_XY;a++) // over all cols a
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++) // over all rows b
        for(int c = 0;c<MATRIX_DIMENSION_XY;c++) // over all rows/cols left
            {
                C[a + b*MATRIX_DIMENSION_XY] += A[c + b*MATRIX_DIMENSION_XY] * B[a + c*MATRIX_DIMENSION_XY]; 
            }
}

void quadratic_matrix_multiplication_parallel(int par_id,int par_count,float *A,float *B,float *C){
    float size = (float)MATRIX_DIMENSION_XY/(float)par_count;
    int start = size*(par_id);
    int end = size*(par_id+1);
    
    
    for(int a = start;a<end;a++) // over all cols a
        for(int b = 0;b<MATRIX_DIMENSION_XY;b++) // over all rows b
            for(int c = 0;c<MATRIX_DIMENSION_XY;c++) // over all rows/cols left
                {
                    C[a + b*MATRIX_DIMENSION_XY] += A[c + b*MATRIX_DIMENSION_XY] * B[a + c*MATRIX_DIMENSION_XY]; 
                }
}

//************************************************************************************************************************
void synch(int par_id,int par_count,int *ready)
{   
    if(par_count>1){
        
    if(par_id==0){
        
        for(int i=1;i<par_count;i++){
            
            while(ready[i]!=1){
                
            }
        }
        
        ready[0]=1;
        for(int i=1;i<par_count;i++){
            while(ready[i]!=0){}
        }
        ready[0]=0;
    }
    else{
        while(ready[0]!=0){}
        ready[par_id] = 1;
        while(ready[0]!=1){}
        ready[par_id]=0;
    }
    
    return;
    }
}
//************************************************************************************************************************
int main(int argc, char *argv[])
{

int par_id = 0; // the parallel ID of this process
int par_count = 1; // the amount of processes
float *A,*B,*C; //matrices A,B and C
int *ready; //needed for synch
if(argc!=3){printf("no shared\n");}
else
    {
    par_id= atoi(argv[1]);
    par_count= atoi(argv[2]);
   // strcpy(shared_mem_matrix,argv[3]);
    }
if(par_count==1){printf("only one process\n");}

int fd[4];

if(par_id==0)
    {
    fd[0] = shm_open("A",O_CREAT|O_RDWR,0600);
    ftruncate(fd[0],sizeof(float)*MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY);
    A = (float*)mmap(NULL,sizeof(float)*MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY,PROT_READ|PROT_WRITE,MAP_SHARED,fd[0],0);
    
    fd[1] = shm_open("B",O_CREAT|O_RDWR,0600);
    ftruncate(fd[1],sizeof(float)*MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY);
    B = (float*)mmap(NULL,sizeof(float)*MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY,PROT_READ|PROT_WRITE,MAP_SHARED,fd[1],0);

    fd[2] = shm_open("C",O_CREAT|O_RDWR,0600);
    ftruncate(fd[2],sizeof(float)*MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY);
    C = (float*)mmap(NULL,sizeof(float)*MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY,PROT_READ|PROT_WRITE,MAP_SHARED,fd[2],0);
    
    fd[3] = shm_open("ready",O_CREAT|O_RDWR,0600);
    ftruncate(fd[3],sizeof(int)*par_count);
    ready = (int*)mmap(NULL,sizeof(int)*par_count,PROT_READ|PROT_WRITE,MAP_SHARED,fd[3],0);
    
    for(int i = 0; i<par_count;i++){
        ready[i]=0;
    }
    }
else
    {
	//TODO: init the shared memory for A,B,C, ready. shm_open withOUT C_CREAT here! NO ftruncate! but yes to mmap
    sleep(2); //needed for initalizing synch
    fd[0] = shm_open("A",O_RDWR,0600);
    A = (float*)mmap(NULL,sizeof(float)*MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY,PROT_READ|PROT_WRITE,MAP_SHARED,fd[0],0);
    
    fd[1] = shm_open("B",O_RDWR,0600);
    B = (float*)mmap(NULL,sizeof(float)*MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY,PROT_READ|PROT_WRITE,MAP_SHARED,fd[1],0);
    
    fd[2] = shm_open("C",O_RDWR,0600);
    C = (float*)mmap(NULL,sizeof(float)*MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY,PROT_READ|PROT_WRITE,MAP_SHARED,fd[2],0);
    
    fd[3] = shm_open("ready",O_RDWR,0600);
    ready = (int*)mmap(NULL,sizeof(int)*par_count,PROT_READ|PROT_WRITE,MAP_SHARED,fd[3],0);
    
    }


synch(par_id,par_count,ready);


if(par_id==0)
    {
	for(int i = 0; i<100; i++){
        A[i]=i+1;
        B[i]= 1+100-i;
    }
    
    }



synch(par_id,par_count,ready);


quadratic_matrix_multiplication_parallel(par_id, par_count,A,B,C);
	
synch(par_id,par_count,ready);

if(par_id==0)
    quadratic_matrix_print(C);
synch(par_id, par_count, ready);

//lets test the result:
float M[MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY];
quadratic_matrix_multiplication(A, B, M);

if (quadratic_matrix_compare(C, M))
	printf("full points!\n");
else
	printf("buuug!\n");



close(fd[0]);
close(fd[1]);
close(fd[2]);
close(fd[3]);
shm_unlink("A");
shm_unlink("B");
shm_unlink("C");
shm_unlink("ready");

return 0;    
}