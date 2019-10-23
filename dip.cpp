#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
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
}  PALET;
typedef struct _IMAGE
{
	BMPFH   bmpfh;
	BMPIH   bmpih;
	PALET   *palet;
	BYTE    *data;
}  IMAGE;
IMAGE *ImageOku(IMAGE *image,char *filename)
{
	BMPFH bmpfh;
	BMPIH bmpih;
	FILE *fp;
	DWORD r,satirsize,size;
	fp=fopen(filename,"rb");
	if(fp==NULL) {printf("Dosya bulunamadý..");exit(1);}
	fread(&bmpfh,sizeof(BMPFH),1,fp);
    fread(&bmpih,sizeof(BMPIH),1,fp);
	image=(IMAGE *) malloc(bmpfh.bfsize);
	if(image==NULL) {printf("Bellek acilamadi..");exit(1);}
	if(!(bmpfh.bftype1=='B' && bmpfh.bftype2=='M'))
	{
		printf("Dosya BMP dosyasý deðil");
		fclose(fp);
		exit(1);
	}
	image->bmpfh=bmpfh;
	image->bmpih=bmpih;

	r=0;
    if(bmpih.bibitcount==1) r=2;
	if(bmpih.bibitcount==4) r=16;
	if(bmpih.bibitcount==8) r=256;
	if(r!=0)
	{
		image->palet=(PALET *) malloc(4*r);
		fread(image->palet,4*r,1,fp);
	}
    
	satirsize=(image->bmpih.biw*image->bmpih.bibitcount+31)/32*4;
	size=satirsize*image->bmpih.bih;
	image->data=(BYTE *)malloc(size);
	fread(image->data,size,1,fp);
	fclose(fp);
	return image;
}
IMAGE *PaletDegis(IMAGE *image)
{
	LONG i,j,r=0;
	if(image->bmpih.bibitcount=1) r=2;
	if(image->bmpih.bibitcount=4) r=16;
	if(image->bmpih.bibitcount=8) r=256;
	if(r)
		for(i=0;i<r;i++)
		{
			image->palet[i].rgbblue=i;
			image->palet[i].rgbgreen=i;
			image->palet[i].rgbred=i;
			image->palet[i].rgbreserved=1;
		}
	return image;
}
void NegatifImage(IMAGE *image)
{
	LONG w,h,satir,i,j;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	satir=((image->bmpih.biw*image->bmpih.bibitcount+31)/32*4);
	for(i=0;i<h;i++)
	   for(j=0;j<satir;j++)
	       image->data[i*satir+j]=255-image->data[i*satir+j];
	return;
}
void Esik(IMAGE *image,LONG E)
{
	LONG w,h,satir,i,j;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	satir=((image->bmpih.biw*image->bmpih.bibitcount+31)/32*4);
	for(i=0;i<h;i++)
	   for(j=0;j<satir;j++)
	       if(image->data[i*satir+j]>E) image->data[i*satir+j]=255;
	       else image->data[i*satir+j]=0;
	return;
	
}
void OrtEsik(IMAGE *image)
{
	LONG w,h,satir,i,j;
	double t=0;
	LONG E;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	satir=((image->bmpih.biw*image->bmpih.bibitcount+31)/32*4);
	for(i=0;i<h;i++)
	   for(j=0;j<satir;j++)
	       t+=image->data[i*satir+j];
	E=t/(h*satir);
	printf("Hesaplanan Esik Degeri=%d\n",E);
	for(i=0;i<h;i++)
	   for(j=0;j<satir;j++)
	       if(image->data[i*satir+j]>E) image->data[i*satir+j]=255;
	       else image->data[i*satir+j]=0;	
}
void ImageYaz(IMAGE *image,char *filename)
{
	FILE *fp;
	int r,satirsize,size;
	fp=fopen(filename,"wb");
	if(fp==NULL) {printf("Dosya hatasý..");exit(1);}
	fwrite(&image->bmpfh,sizeof(BMPFH),1,fp);
	fwrite(&image->bmpih,sizeof(BMPIH),1,fp);		
	r=0;
    if(image->bmpih.bibitcount==1) r=2;
	if(image->bmpih.bibitcount==4) r=16;
	if(image->bmpih.bibitcount==8) r=256;
	if(r!=0) fwrite(image->palet,4*r,1,fp);
	satirsize=(image->bmpih.biw*image->bmpih.bibitcount+31)/32*4;
	size=satirsize*image->bmpih.bih;
	fwrite(image->data,size,1,fp);
	fclose(fp);
}
void ImageInfo(IMAGE *image)
{
     printf("\n--------------- BMP Dosya Bilgileri --------------");
     printf("\n Tip(ilk Karakter)                  :%c",image->bmpfh.bftype1);
     printf("\n Tip(ikinci Karakter)               :%c",image->bmpfh.bftype2);
     printf("\n Dosya Büyüklügü                    :%d",image->bmpfh.bfsize);
     printf("\n 1. reserv alaný                    :%d",image->bmpfh.bfreserved1);
     printf("\n 2. reserv alaný                    :%d",image->bmpfh.bfreserved2);
     printf("\n Data baslangic yeri                :%d",image->bmpfh.bfOffbits);
     printf("\n--------------- Goruntuye ait Bilgiler --------------");
     printf("\n Goruntu Baslik Buyuklugu           :%d",image->bmpih.bisize);
     printf("\n Goruntu Genisligi                  :%d",image->bmpih.biw);
     printf("\n Goruntu Yuksekligi                 :%d",image->bmpih.bih);
     printf("\n Goruntu Duzlem Degeri              :%d",image->bmpih.biplane);
     printf("\n 1 pixelin kac bit degeri           :%d",image->bmpih.bibitcount);
     printf("\n 1 pixelin compression degeri       :%d",image->bmpih.biComp);	
     printf("\n 1 Data Buyuklugu                   :%d",image->bmpih.bisizeimage);
     printf("\n dusey cozunurluk                   :%d",image->bmpih.bix);
     printf("\n yatay cozunurluk                   :%d",image->bmpih.biy);
     printf("\n Kullanilan renk sayýsý             :%d",image->bmpih.biclused);
     printf("\n Renk Onem Degeri                   :%d",image->bmpih.biclimp);	
     return;
} 
void BolgeselEsik(IMAGE *image,LONG N) 
{
	LONG w,h,satir,i,j,k,n;
	double t=0;
	LONG E;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	satir=((image->bmpih.biw*image->bmpih.bibitcount+31)/32*4);
	for(i=0;i<h;i+=N)
	   for(j=0;j<satir;j+=N)
	   {
	       t=0;
	       for(k=0;k<N;k++)
	          for(n=0;n<N;n++)
	              t+=image->data[(i+k)*satir+j+n];
	        E=t/(N*N);
	        for(k=0;k<N;k++)
	          for(n=0;n<N;n++)
	              if(image->data[(i+k)*satir+j+n]>E)image->data[(i+k)*satir+j+n]=255;
	              else image->data[(i+k)*satir+j+n]=0;
	    }
	return;
} 
void maxmin(IMAGE *image,BYTE *min,BYTE *max)
{
	LONG w,h,satir,i;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	satir=((image->bmpih.biw*image->bmpih.bibitcount+31)/32*4);
	*min=255; *max=0;
	for(i=0;i<satir*h;i++)
	{
		if(*min>image->data[i]) *min=image->data[i];
		if(*max<image->data[i]) *max=image->data[i];
	}
	return;	
}
void Histogram(IMAGE *image,int hist[]) 
{

	FILE *fp;
	LONG w,h,satir,i;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	satir=((image->bmpih.biw*image->bmpih.bibitcount+31)/32*4);
	fp=fopen("histogram.txt","w");
	for(i=0;i<256;i++) hist[i]=0;
	for(i=0;i<satir*h;i++)
	      hist[image->data[i]]++;
	for(i=0;i<256;i++)
	   fprintf(fp,"%d\t%d\n",i,hist[i]);
	fclose(fp);
	return;
}  
void HistogramDegis(IMAGE *image,BYTE MIN,BYTE MAX)
{
	LONG w,h,satir,i;
	int hist[256];
	BYTE min,max;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	satir=((image->bmpih.biw*image->bmpih.bibitcount+31)/32*4);
	
	maxmin(image,&min,&max);
	printf("min=%d max=%d\n",min,max);
	for(i=0;i<h*satir;i++)
	   image->data[i]=(MAX-MIN)*(image->data[i]-min)/(max-min)+MIN;
	Histogram(image,hist);
	return;	
}
void HistogramEsitleme(IMAGE *image)   
{
	LONG w,h,satir,i;
	int hist[256];
	BYTE min,max;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	satir=((image->bmpih.biw*image->bmpih.bibitcount+31)/32*4);
	Histogram(image,hist);
	double t=0;
	for(i=0;i<256;i++)
	{
		t+=hist[i];
		hist[i]=(t/(h*satir)*255+0.5);
	}
	for(i=0;i<h*satir;i++)
	       image->data[i]=hist[image->data[i]];
	return;	
}                 
int main()
{
	IMAGE *image;
	image=ImageOku(image,"biber.bmp");
    HistogramEsitleme(image);
 	ImageYaz(image,"temp.bmp");
    free(image);	
    system("pause");
	return 0;
}
 



