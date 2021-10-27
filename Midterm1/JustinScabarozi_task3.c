#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
struct tagBITMAPFILEHEADER{
    WORD bfType;  //specifies the file type
    DWORD bfSize;  //specifies the size in bytes of the bitmap file
    WORD bfReserved1;  //reserved; must be 0
    WORD bfReserved2;  //reserved; must be 0
    DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
    };

struct tagBITMAPINFOHEADER{
    DWORD biSize;  //specifies the number of bytes required by the struct
    LONG biWidth;  //specifies width in pixels
    LONG biHeight;  //species heightin pixels
    WORD biPlanes; //specifies the number of color planes, must be 1
    WORD biBitCount; //specifies the number of bit per pixel
    DWORD biCompression;//spcifies the type of compression
    DWORD biSizeImage;  //size of image in bytes
    LONG biXPelsPerMeter;  //number of pixels per meter in x axis
    LONG biYPelsPerMeter;  //number of pixels per meter in y axis
    DWORD biClrUsed;  //number of colors used by th ebitmap
    DWORD biClrImportant;  //number of colors that are important
    };


void myread(struct tagBITMAPFILEHEADER *header, struct tagBITMAPINFOHEADER *info, FILE *file, unsigned char **pic){
    fread(&header->bfType,sizeof(WORD),1,file);
    fread(&header->bfSize,sizeof(DWORD),1,file);
    fread(&header->bfReserved1,sizeof(WORD),1,file);
    fread(&header->bfReserved2,sizeof(WORD),1,file);
    fread(&header->bfOffBits,sizeof(DWORD),1,file);

    fread(info,sizeof(struct tagBITMAPINFOHEADER),1,file);
    
    *pic = (unsigned char*)malloc(info->biSizeImage);
    fread(*pic,info->biSizeImage,1,file);
}

int GetTimeStamp(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*1000000+tv.tv_usec;
}

void main(){
    struct tagBITMAPFILEHEADER header;
    struct tagBITMAPINFOHEADER info;
    struct tagBITMAPFILEHEADER header2;
    struct tagBITMAPINFOHEADER info2;


    unsigned char *pic;
    unsigned char *pic2;

    int status;
    int start[5];
    int end[5];
    
    FILE *new = fopen("new.bmp","wb");
    FILE *file = fopen("nopadding.bmp", "rb");
    FILE *file2 = fopen("yespadding.bmp","rb");
    float ratio = .5;
    int smallwidth;

    myread(&header,&info,file,&pic);
    myread(&header2,&info2,file2,&pic2);

    unsigned char *merge = mmap(NULL,sizeof(unsigned char)*info.biSizeImage,PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON,-1,0);

    int realwidth = info.biWidth*3;
    if(realwidth%4!=0)
        realwidth = realwidth +4 - realwidth%4;

    float wratio = ((float)info2.biWidth/(float)info.biWidth);
    float hratio = ((float)info2.biHeight/(float)info.biHeight);
    smallwidth = info2.biWidth*3;
    if(smallwidth%4!=0)
        smallwidth = smallwidth +4 - smallwidth%4;
    
    start[4] = GetTimeStamp();
    start[0] = GetTimeStamp();

    pid_t pid1 = fork();
    if(pid1==0){
        

        for(int y = 0; y<info.biHeight/2;y++){
            for (int x = 0; x<info.biWidth/2; x++)
            {
                float xs = x * wratio;
                int x1 = xs;
                int x2 = x1;
                if(xs-(float)x1!=0)
                    x2 = xs+1;
                

                float ys = y * hratio;
                int y1 = ys;
                int y2 = y1;
                if(ys-(float)y1!=0)
                    y2 = ys+1;

                if(x2*3>=smallwidth-1)
                    x2 = x2-1;
                if(y2>=info2.biHeight)
                    y2 = y2-1;

                

                int pixel = 3*x+y*realwidth;
                int pixel2 = 3*x2+y2*smallwidth;
                merge[pixel] = ratio*pic[pixel] + (1-ratio)* pic2[pixel2];
                merge[pixel+1] = ratio*pic[pixel+1] + (1-ratio)* pic2[pixel2+1];
                merge[pixel+2] = ratio*pic[pixel+2] + (1-ratio)* pic2[pixel2+2];
            }

        }
        return;  
    }

    start[1] = GetTimeStamp();
    pid_t pid2 = fork();
    if(pid2==0){
        

        for(int y = 0; y<info.biHeight/2;y++){
            for (int x = info.biWidth/2; x<info.biWidth; x++)
            {
                float xs = x * wratio;
                int x1 = xs;
                int x2 = x1;
                if(xs-(float)x1!=0)
                    x2 = xs+1;
                

                float ys = y * hratio;
                int y1 = ys;
                int y2 = y1;
                if(ys-(float)y1!=0)
                    y2 = ys+1;

                if(x2*3>=smallwidth-1)
                    x2 = x2-1;
                if(y2>=info2.biHeight)
                    y2 = y2-1;

                

                int pixel = 3*x+y*realwidth;
                int pixel2 = 3*x2+y2*smallwidth;
                merge[pixel] = ratio*pic[pixel] + (1-ratio)* pic2[pixel2];
                merge[pixel+1] = ratio*pic[pixel+1] + (1-ratio)* pic2[pixel2+1];
                merge[pixel+2] = ratio*pic[pixel+2] + (1-ratio)* pic2[pixel2+2];
            }

        }
        return;  
    }

    start[2] = GetTimeStamp();
    pid_t pid3 = fork();
    if(pid3==0){
        

        for(int y = info.biHeight/2; y<info.biHeight;y++){
            for (int x = 0; x<info.biWidth/2; x++)
            {
                float xs = x * wratio;
                int x1 = xs;
                int x2 = x1;
                if(xs-(float)x1!=0)
                    x2 = xs+1;
                

                float ys = y * hratio;
                int y1 = ys;
                int y2 = y1;
                if(ys-(float)y1!=0)
                    y2 = ys+1;

                if(x2*3>=smallwidth-1)
                    x2 = x2-1;
                if(y2>=info2.biHeight)
                    y2 = y2-1;

                

                int pixel = 3*x+y*realwidth;
                int pixel2 = 3*x2+y2*smallwidth;
                merge[pixel] = ratio*pic[pixel] + (1-ratio)* pic2[pixel2];
                merge[pixel+1] = ratio*pic[pixel+1] + (1-ratio)* pic2[pixel2+1];
                merge[pixel+2] = ratio*pic[pixel+2] + (1-ratio)* pic2[pixel2+2];
            }

        }
        return;  
    }

    int abc;

    start[3] = GetTimeStamp();
    for(int y = info.biHeight/2; y<info.biHeight;y++){
        for (int x = info.biWidth/2; x<info.biWidth; x++)
        {
            if(x==info.biWidth-1){
                abc=11;
            }
            float xs = x * wratio;
            int x1 = xs;
            int x2 = x1;
            if(xs-(float)x1!=0)
                x2 = xs+1;
            
            float ys = y * hratio;
            int y1 = ys;
            int y2 = y1;
            if(ys-(float)y1!=0)
                y2 = ys+1;
                
            if(x2*3>=smallwidth-1)
                x2 = x2-1;
            if(y2>=info2.biHeight)
                y2 = y2-1;
            
            int pixel = 3*x+y*realwidth;
            int pixel2 = 3*x2+y2*smallwidth;
            merge[pixel] = ratio*pic[pixel] + (1-ratio)* pic2[pixel2];
            merge[pixel+1] = ratio*pic[pixel+1] + (1-ratio)* pic2[pixel2+1];
            merge[pixel+2] = ratio*pic[pixel+2] + (1-ratio)* pic2[pixel2+2];
        }
    }
    end[3] = GetTimeStamp();
        
    
    waitpid(pid1,&status,0);
    end[0] = GetTimeStamp();
    waitpid(pid2,&status,0);
    end[1] = GetTimeStamp();
    waitpid(pid3,&status,0);
    end[2] = GetTimeStamp();
    end[4] = GetTimeStamp();

    fwrite(&header.bfType,sizeof(WORD),1,new);
    fwrite(&header.bfSize,sizeof(DWORD),1,new);
    fwrite(&header.bfReserved1,sizeof(WORD),1,new);
    fwrite(&header.bfReserved2,sizeof(WORD),1,new);
    fwrite(&header.bfOffBits,sizeof(DWORD),1,new);
    fwrite(&info,sizeof(struct tagBITMAPINFOHEADER),1,new);
    fwrite(merge,info.biSizeImage,1,new);
    
    fclose(new);
    fclose(file);

    munmap(merge,info.biSizeImage*sizeof(unsigned char));

    printf("process 1: %d\n", end[0]-start[0]);
    printf("process 2: %d\n", end[1]-start[1]);
    printf("process 3: %d\n", end[2]-start[2]);
    printf("process 4: %d\n", end[3]-start[3]);
    printf("process total: %d\n", end[4]-start[4]);

}