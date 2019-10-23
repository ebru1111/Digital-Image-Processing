#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>
#include<string.h>
#include <fcntl.h>
#include <malloc.h>
#include <math.h>
#define PI 3.1415926535897932384626433832795
#include "structs.h"
#include "image.h"
#include "common.h"
#include "hsi.h"
#include "ycbcr.h"
#include "hsl.h"
#include "hsv.h"
#include "yuv.h"
#include "ypbpr.h"
#include "hypbpr.h"
#include "yiq.h"
#include "cmyk.h"
void RGB2HYCbCr(struct ppm_file *image,struct YCbCr *ycbcr); //Converts from RGB into YCbCr (HDTV)
void HYCbCr2RGB(struct ppm_file *image,struct YCbCr *ycbcr); //Converts from YCbCr into RGB (HDTV)
void HYCbCrEqualization(char *fname); //YCbCr Equalization (HDTV)
//Converts from RGB into YCbCr (HDTV)
void RGB2HYCbCr(struct ppm_file *image,struct YCbCr *ycbcr)
{
	int k,height,width,x,y,z;
	float r,g,b,i,s,h;
	height=image->pheader->pheight;
	width=image->pheader->pwidth;
    ycbcr->Y=(short int*)malloc(height*width*sizeof(short int));
    ycbcr->Cb=(short int*)malloc(height*width*sizeof(short int));
    ycbcr->Cr=(short int*)malloc(height*width*sizeof(short int));
    if(ycbcr->Y==NULL || ycbcr->Cb==NULL || ycbcr->Cr==NULL)
    {
    	printf("\nMemory error");
		exit(1);
	}
	for(k=0;k<height*width;k++)
	{
		x=(16+0.183*image->rdata[k]+0.614*image->gdata[k]+0.062*image->bdata[k]);
		y=(128-0.101*image->rdata[k]-0.339*image->gdata[k]+0.439*image->bdata[k]);
		z=(128+0.439*image->rdata[k]-0.399*image->gdata[k]-0.040*image->bdata[k]);
		ycbcr->Y[k]=x;
		ycbcr->Cb[k]=y;
		ycbcr->Cr[k]=z;
	}
	return;
	
}
//Converts from YCbCr into RGB (HDTV)
void HYCbCr2RGB(struct ppm_file *image,struct YCbCr *ycbcr)
{
	int k;
	short int x,y,z;
	double MSEr=0,MSEg=0,MSEb=0;
    for(k=0;k<image->pheader->pheight*image->pheader->pwidth;k++)
	{
		x=1.164*(ycbcr->Y[k]-16)+1.793*(ycbcr->Cr[k]-128);
		y=1.164*(ycbcr->Y[k]-16)-0.213*(ycbcr->Cb[k]-128)-0.533*(ycbcr->Cr[k]-128);
		z=1.164*(ycbcr->Y[k]-16)+2.212*(ycbcr->Cb[k]-128);
		if(x>255) x=255;
		if(y>255) y=255;
		if(z>255) z=255;
		if(x<0) x=0;
		if(y<0) y=0;
		if(z<0) z=0;
		MSEr+=pow(image->rdata[k]-x,2);
		MSEg+=pow(image->gdata[k]-y,2);
		MSEb+=pow(image->bdata[k]-z,2);
		image->rdata[k]=(unsigned char) x;
		image->gdata[k]=(unsigned char) y;
		image->bdata[k]=(unsigned char) z;
	}
    MSEr=MSEr/(image->pheader->pheight*image->pheader->pwidth);
	MSEg=MSEg/(image->pheader->pheight*image->pheader->pwidth);
	MSEb=MSEb/(image->pheader->pheight*image->pheader->pwidth);
    //Calculation of PSNR value for each channel 
	ycbcr->psnr.rvalue=20*log10(255./sqrt(MSEr));
	ycbcr->psnr.gvalue=20*log10(255./sqrt(MSEg));
	ycbcr->psnr.bvalue=20*log10(255./sqrt(MSEb));
	return;
}
//YCbCr Equalization (HDTV)
void HYCbCrEqualization(char *fname)
{
	char *newfile=(char *)malloc(30);
	struct ppm_file *resim=(ppm_file*)malloc(sizeof(ppm_file));
	if(resim==NULL)
	{
	    printf("\nMemory Problem!");exit(1);
	}
	int i;
	double average;
	struct YCbCr  *ycbcr=(struct YCbCr*)malloc(sizeof(struct YCbCr));
	if(ycbcr==NULL)
	{
		printf("\nMemory error");
		exit(1);
	}
	get_image_data(fname,resim);

	//Converts from RGB into YCbCr (HDTV)
	RGB2HYCbCr(resim,ycbcr);
	//Histogram equalization on channel Y (HDTV)
	equalization(ycbcr->Y,resim->pheader->pwidth*resim->pheader->pheight);
	//Converts from YCbCr into RGB (HDTV)
	HYCbCr2RGB(resim,ycbcr);
	average=(ycbcr->psnr.rvalue+ycbcr->psnr.gvalue+ycbcr->psnr.bvalue)/3.;
	printf("\n-------PSNR VALUES OF EACH CHANNEL ----------\n");
	printf("\nR Channel    :%lf",ycbcr->psnr.rvalue);
	printf("\nG Channel    :%lf",ycbcr->psnr.gvalue);
	printf("\nB Channel    :%lf",ycbcr->psnr.bvalue);
	printf("\nAverage Value:%lf",average); 
	//Histogram Equalized image
	i=0;
	while(fname[i]!='.') 
	{
		newfile[i]=fname[i];
		i++;
	}
	newfile[i]='\0';
	strcat(newfile,"_hycbcrequ.ppm");
	write_image(newfile,resim);
	free(ycbcr->Y);free(ycbcr->Cb);free(ycbcr->Cr);free(ycbcr);free(resim);
	system("pause");
	return;
}
int main()
{	
    char *fname=(char *)malloc(30);
    printf("File Name is:");
    gets(fname);
    //strcpy(fname,"mandrill1.ppm");
	int ch;
	while(1)
	{
		system("cls");
		menu();
		printf("\t");
		scanf("%d",&ch);
		switch(ch)
		{
			case 1:
			    HSIEqualization(fname);
			    break;
			case 2:
				YCbCrEqualization(fname);
			case 3:
				HYCbCrEqualization(fname);
			case 4:
				HSVEqualization(fname);
			    break;
			case 5:
				HSLEqualization(fname);
			    break;
			case 6:
				YUVEqualization(fname);
			    break;
			case 7:
				YPbPrEqualization(fname);
			    break;
			case 8:
				HYPbPrEqualization(fname);
			    break;
			case 9:
				YIQEqualization(fname);
			    break;
			case 10:
				CMYKEqualization(fname);
			    break;
			default:
				break;
	    } 
		if(ch==11) break;
    } 
	return 0;
} 
