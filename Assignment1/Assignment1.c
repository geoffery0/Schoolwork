#include <stdio.h>
#include <stdlib.h>

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

void main(){
    struct tagBITMAPFILEHEADER header;
    struct tagBITMAPINFOHEADER info;
    struct tagBITMAPFILEHEADER header2;
    struct tagBITMAPINFOHEADER info2;

    unsigned char *pic;
    unsigned char *pic2;
    unsigned char *merge;
    FILE *new = fopen("new.bmp","wb");
    FILE *file = fopen("tunnel.bmp", "rb");
    FILE *file2 = fopen("flowers.bmp","rb");
    float ratio = .3;
    int realwidth;
    int smallwidth;
    myread(&header,&info,file,&pic); //This one is bigger although it doesnt seem to matter
    myread(&header2,&info2,file2,&pic2);
    merge = (unsigned char*)malloc(info.biSizeImage);
    realwidth = info.biWidth*3;
    if(realwidth%4!=0)
        realwidth = realwidth +4 - realwidth%4;
    if(info.biWidth==info2.biWidth){
        

        for(int y = 0; y<info.biHeight;y++){
            for (int x = 0; x<info.biWidth; x++)
            {
                int pixel = 3*x+y*realwidth;
                merge[pixel] = ratio*pic[pixel] + (1-ratio)* pic2[pixel];
                merge[pixel+1] = ratio*pic[pixel+1] + (1-ratio)* pic2[pixel+1];
                merge[pixel+2] = ratio*pic[pixel+2] + (1-ratio)* pic2[pixel+2];
            }

        }
    }else{
        float wratio = ((float)info2.biWidth/(float)info.biWidth);
        float hratio = ((float)info2.biHeight/(float)info.biHeight);
        smallwidth = info2.biWidth*3;
        if(smallwidth%4!=0)
            smallwidth = smallwidth +4 - smallwidth%4;

        for(int y = 0; y<info.biHeight;y++){
            for (int x = 0; x<info.biWidth; x++)
            {
                float xs = x * wratio;
                int x1 = xs;
                int x2 = xs+1;
                float dx = xs-x1;

                float ys = y * hratio;
                int y1 = ys;
                int y2 = ys+1;
                float dy = ys-y1;

                /*if(x2>smallwidth)
                    x2 = x1;
                if(y2>info2.biHeight)
                    y2 = y1;*/

                unsigned char up1 = (1-dx) * pic2[x1*3+y1*smallwidth]+dx*pic2[x2*3+y1*smallwidth];
                unsigned char down1 = (1-dx) * pic2[x1*3+y2*smallwidth]+dx*pic2[x2*3+y2*smallwidth];
                unsigned char whole1 = (1-dy)*up1+dy*down1;

                unsigned char up2 = (1-dx) * pic2[x1*3+y1*smallwidth+1]+dx*pic2[x2*3+y1*smallwidth+1];
                unsigned char down2 = (1-dx) * pic2[x1*3+y2*smallwidth+1]+dx*pic2[x2*3+y2*smallwidth+1];
                unsigned char whole2 = (1-dy)*up2+dy*down2;

                unsigned char up3 = (1-dx) * pic2[x1*3+y1*smallwidth+2]+dx*pic2[x2*3+y1*smallwidth+2];
                unsigned char down3 = (1-dx) * pic2[x1*3+y2*smallwidth+2]+dx*pic2[x2*3+y2*smallwidth+2];
                unsigned char whole3 = (1-dy)*up3+dy*down3;

                int pixel = 3*x+y*realwidth;
                merge[pixel] = ratio*pic[pixel] + (1-ratio)* whole1;
                merge[pixel+1] = ratio*pic[pixel+1] + (1-ratio)* whole2;
                merge[pixel+2] = ratio*pic[pixel+2] + (1-ratio)* whole3;
            }

        }
    }



    fwrite(&header.bfType,sizeof(WORD),1,new);
    fwrite(&header.bfSize,sizeof(DWORD),1,new);
    fwrite(&header.bfReserved1,sizeof(WORD),1,new);
    fwrite(&header.bfReserved2,sizeof(WORD),1,new);
    fwrite(&header.bfOffBits,sizeof(DWORD),1,new);
    fwrite(&info,sizeof(struct tagBITMAPINFOHEADER),1,new);
    fwrite(merge,info.biSizeImage,1,new);
    
    fclose(new);
    fclose(file);
    fclose(file2);
    free(pic2);
    free(pic);
    free(merge);
}