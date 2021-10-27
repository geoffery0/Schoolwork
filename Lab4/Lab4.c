#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

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

unsigned char set(int numb){
    if(numb>255)
        numb = 255;
    return (unsigned char)numb;
}


void main(int argc, char *argv[]){
    struct tagBITMAPFILEHEADER header;
    struct tagBITMAPINFOHEADER info;


    char *imagename = argv[1];
    float brightness = atof(argv[2]);
    int parallel = atoi(argv[3]);
    char *output = argv[4];

    unsigned char *pic;


    FILE *new = fopen(output,"wb");
    FILE *file = fopen(imagename, "rb");

    myread(&header,&info,file,&pic);

    unsigned char *bright = mmap(NULL,sizeof(unsigned char)*info.biSizeImage,PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON,-1,0);

    int realwidth = info.biWidth*3;
    if(realwidth%4!=0)
        realwidth = realwidth +4 - realwidth%4;


    if(parallel == 0){

        for(int y = 0; y<info.biHeight;y++){
            for (int x = 0; x<info.biWidth; x++)
            {
                int pixel = 3*x+y*realwidth;
                int first = (int)pic[pixel]+255*brightness;
                int second = (int)pic[pixel+1]+255*brightness;
                int third = (int)pic[pixel+2]+255*brightness;
                
                bright[pixel] = set(first);
                bright[pixel+1] = set(second);
                bright[pixel+2] = set(third);
            }

        }
    }else{
        if(fork()==0){
            for(int y = 0; y<info.biHeight;y++){
                for (int x = info.biWidth/2; x<info.biWidth; x++)
                {
                    int pixel = 3*x+y*realwidth;
                    int first = (int)pic[pixel]+255*brightness;
                    int second = (int)pic[pixel+1]+255*brightness;
                    int third = (int)pic[pixel+2]+255*brightness;

                    bright[pixel] = set(first);
                    bright[pixel+1] = set(second);
                    bright[pixel+2] = set(third);
                }

            }
            return;  
        }else{
            for(int y = 0; y<info.biHeight;y++){
                for (int x = 0; x<info.biWidth/2; x++)
                {
                    int pixel = 3*x+y*realwidth;
                    int first = (int)pic[pixel]+255*brightness;
                    int second = (int)pic[pixel+1]+255*brightness;
                    int third = (int)pic[pixel+2]+255*brightness;

                    bright[pixel] = set(first);
                    bright[pixel+1] = set(second);
                    bright[pixel+2] = set(third);
                }

            } 
        }
        
    }
    wait(0);

    fwrite(&header.bfType,sizeof(WORD),1,new);
    fwrite(&header.bfSize,sizeof(DWORD),1,new);
    fwrite(&header.bfReserved1,sizeof(WORD),1,new);
    fwrite(&header.bfReserved2,sizeof(WORD),1,new);
    fwrite(&header.bfOffBits,sizeof(DWORD),1,new);
    fwrite(&info,sizeof(struct tagBITMAPINFOHEADER),1,new);
    fwrite(bright,info.biSizeImage,1,new);
    
    fclose(new);
    fclose(file);

    munmap(bright,info.biSizeImage*sizeof(unsigned char));

}