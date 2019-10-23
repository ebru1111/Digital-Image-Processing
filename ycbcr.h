void RGB2YCbCr(struct ppm_file *image,struct YCbCr *ycbcr); //Converts from RGB into YCbCr (SDTV)
void YCbCr2RGB(struct ppm_file *image,struct YCbCr *ycbcr); //Converts from YCbCr into RGB (SDTV)
void YCbCrEqualization(char *fname); //YCbCr Equalization (SDTV)
//Converts from RGB into YCbCr (SDTV)
void RGB2YCbCr(struct ppm_file *image,struct YCbCr *ycbcr)
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
		x=(16+0.257*image->rdata[k]+0.504*image->gdata[k]+0.098*image->bdata[k]);
		y=(128-0.148*image->rdata[k]-0.291*image->gdata[k]+0.439*image->bdata[k]);
		z=(128+0.439*image->rdata[k]-0.368*image->gdata[k]-0.071*image->bdata[k]);
		ycbcr->Y[k]= (16+0.257*image->rdata[k]+0.504*image->gdata[k]+0.098*image->bdata[k]);
		ycbcr->Cb[k]= (128-0.148*image->rdata[k]-0.291*image->gdata[k]+0.439*image->bdata[k]);
		ycbcr->Cr[k]= (128+0.439*image->rdata[k]-0.368*image->gdata[k]-0.071*image->bdata[k]);
	}
	return;
	
}
//Converts from YCbCr into RGB (SDTV)
void YCbCr2RGB(struct ppm_file *image,struct YCbCr *ycbcr)
{
	int k;
	short int x,y,z;
	double MSEr=0,MSEg=0,MSEb=0;
    for(k=0;k<image->pheader->pheight*image->pheader->pwidth;k++)
	{
		x=1.164*(ycbcr->Y[k]-16)+1.596*(ycbcr->Cr[k]-128);
		y=1.164*(ycbcr->Y[k]-16)-0.392*(ycbcr->Cb[k]-128)-0.813*(ycbcr->Cr[k]-128);
		z=1.164*(ycbcr->Y[k]-16)+2.017*(ycbcr->Cb[k]-128);
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
//YCbCr Equalization (SDTV)
void YCbCrEqualization(char *fname)
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

	//Converts from RGB into YCbCr (SDTV)
	RGB2YCbCr(resim,ycbcr);
	//Histogram equalization on channel Y (SDTV)
	equalization(ycbcr->Y,resim->pheader->pwidth*resim->pheader->pheight);
	//Converts from YCbCr into RGB (SDTV)
	YCbCr2RGB(resim,ycbcr);
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
	strcat(newfile,"_ycbcrequ.ppm");
	write_image(newfile,resim);
	free(ycbcr->Y);free(ycbcr->Cb);free(ycbcr->Cr);free(ycbcr);free(resim);
	system("pause");
	return;
}
