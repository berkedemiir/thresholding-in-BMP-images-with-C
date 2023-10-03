#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <math.h>
#pragma pack(1)
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int LONG;
typedef struct _BMPFH
{
	BYTE bftype1;
	BYTE bftype2;
	DWORD bfsize;
	WORD bfreserved1;
	WORD bfreserved2;
	DWORD bfOffbits;
} BMPFH;
typedef struct _BMPIH
{
	DWORD bisize;
	LONG  biw;
	LONG bih;
	WORD biplane;
	WORD bibitcount;
	DWORD biComp;
	DWORD bisizeimage;
	LONG bix;
	LONG biy;
	DWORD biclused;
 	DWORD biclimp;
}  BMPIH;
typedef struct _PALET
{
	BYTE rgbblue;
	BYTE rgbgreen;
	BYTE rgbred;
	BYTE rgbreserved;
} PALET;
typedef struct _IMAGE
{
	BMPFH   bmpfh;
	BMPIH   bmpih;
	PALET   *palet;
	BYTE    *data;
}  IMAGE;
IMAGE *ImageRead(IMAGE *image,char *filename)
{
	BMPFH bmpfh;
	BMPIH bmpih;
	FILE *fp;
	DWORD r,rowsize,size;
	fp=fopen(filename,"rb");//tries to open the filename
	if(fp==NULL) {printf("File is not found..");exit(1);}
	fread(&bmpfh,sizeof(BMPFH),1,fp);//reads bitmap file header from the file and set to bmph
	if(bmpfh.bftype1!='B' || bmpfh.bftype2!='M')
	{
		printf("This is not a bitmap file.");
		exit(1);
	}
    fread(&bmpih,sizeof(BMPIH),1,fp);//reads bitmap info header from the file and set to bmpih
	image=(IMAGE *) malloc(bmpfh.bfsize);
	if(image==NULL) {printf("There is no enough memory for this operation");exit(1);}
	image->bmpfh=bmpfh;
	image->bmpih=bmpih;
	
	r=0;
    if(bmpih.bibitcount==1) r=2;//if the image 1 bit per pixel (the number of clor is 2)
	if(bmpih.bibitcount==4) r=16;//if the image 4 bits per pixel (the number of clor is 16)
	if(bmpih.bibitcount==8) r=256;////if the image 8 bits per pixel (the number of clor is 256)
	if(r!=0)
	{
		image->palet=(PALET *) malloc(4*r);
		fread(image->palet,sizeof(BYTE),4*r,fp);
	}
	rowsize=(image->bmpih.biw*image->bmpih.bibitcount+31)/32*4;//calculates 1 row of image size 
	size=rowsize*image->bmpih.bih;
	image->data=(BYTE *)malloc(size);
	fread(image->data,size,1,fp);
	fclose(fp); 
	return image;
}       
void ImageWrite(IMAGE *image,char *filename)
{
	FILE *fp;
	int r,rowsize,size;
	
	fp=fopen(filename,"wb");
	if(fp==NULL) {printf("Fie opening error!");exit(1);}
	fwrite(&image->bmpfh,sizeof(BMPFH),1,fp);
	fwrite(&image->bmpih,sizeof(BMPIH),1,fp);	
	r=0;
    if(image->bmpih.bibitcount==1) r=2;//if the image has 2 colors
	if(image->bmpih.bibitcount==4) r=16;//if the image has 16 colors
	if(image->bmpih.bibitcount==8) r=256;//if the image has 256 colors
	if(r!=0) fwrite(image->palet,4*r,1,fp);//if the image has color palette the palette is written to the file
	rowsize=(image->bmpih.biw*image->bmpih.bibitcount+31)/32*4;
	size=rowsize*image->bmpih.bih;
	fwrite(image->data,size,1,fp);
	fclose(fp);
}
void writeInfo(IMAGE *image,char *fname)
{
	printf("--------Info about %s  image file\n",fname);
	printf("bfType value		:%c%c\n",image->bmpfh.bftype1,image->bmpfh.bftype2);
	printf("bfsize			:%d\n",image->bmpfh.bfsize);
	printf("bfreserved1		:%d\n",image->bmpfh.bfreserved1);
	printf("bfreserved2		:%d\n",image->bmpfh.bfreserved2);
	printf("bfOffbits		:%d\n",image->bmpfh.bfOffbits);
	printf("bisize			:%d\n",image->bmpih.bisize);
	printf("Width			:%d\n",image->bmpih.biw);
	printf("Height			:%d\n",image->bmpih.bih);
	printf("biplane			:%d\n",image->bmpih.biplane);
	printf("bibitcount		:%d\n",image->bmpih.bibitcount);
	printf("Compression		:%d\n",image->bmpih.biComp);
	printf("bisizeimage		:%d\n",image->bmpih.bisizeimage);
	printf("bix			:%d\n",image->bmpih.bix);
	printf("biy			:%d\n",image->bmpih.biy);
	printf("bi color used		:%d\n",image->bmpih.biclused);
	printf("bi color imp.		:%d\n",image->bmpih.biclimp);
}
void thresholdImage(IMAGE *image,BYTE t)
{
	int h,w,rowsize,i;
	h=image->bmpih.bih;
	w=image->bmpih.biw;
	rowsize=(image->bmpih.bibitcount*w+31)/32*4;
	for(i=0;i<h*rowsize;i++) 
	    if (image->data[i]>t) image->data[i]=255;
	    else image->data[i]=0;
}

int main()
{
	IMAGE *image=(IMAGE*)malloc(sizeof(IMAGE));
	image=ImageRead(image,"Charles.bmp");
	writeInfo(image,"CharlesThreshold.bmp");
    thresholdImage(image,127);
	ImageWrite(image,"CharlesThreshold.bmp");
	free(image);   
	
	return 0;
}
