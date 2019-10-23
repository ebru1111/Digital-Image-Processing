void RGB2YUV(struct ppm_file *image,struct YCbCr *ycbcr); //Converts from RGB into YUV
void YUV2RGB(struct ppm_file *image,struct YCbCr *ycbcr); //Converts from YUV into RGB
void YUVEqualization(char *fname); //YUV Equalization
//Converts from RGB into YUV
void RGB2YUV(struct ppm_file *image,struct YUV *yuv)
{
	int k,height,width;
	float r,g,b;
	height=image->pheader->pheight;
	width=image->pheader->pwidth;
    yuv->Y=(short int*)malloc(height*width*sizeof(short int));
    yuv->U=(float*)malloc(height*width*sizeof(float));
    yuv->V=(float*)malloc(height*width*sizeof(float));
    if(yuv->Y==NULL || yuv->U==NULL || yuv->V==NULL)
    {
    	printf("\nMemory error");
		exit(1);
	}
	for(k=0;k<height*width;k++)
	{
		r=image->rdata[k]/255.;
		g=image->gdata[k]/255.;
		b=image->bdata[k]/255.;
		yuv->Y[k]=round((0.299*r+0.587*g+0.114*b)*255);
		yuv->U[k]=-0.147*r-0.289*g+0.436*b;
		yuv->V[k]=0.615*r-0.515*g-0.1*b;
	}
	return;
	
}
//Converts from YUV into RGB
void YUV2RGB(struct ppm_file *image,struct YUV *yuv)
{
	int k;
	float r,g,b;
	double MSEr=0,MSEg=0,MSEb=0;
    for(k=0;k<image->pheader->pheight*image->pheader->pwidth;k++)
	{
		r=(yuv->Y[k]/255.+1.14*yuv->V[k])*255;
		g=(yuv->Y[k]/255.-0.395*yuv->U[k]-0.581*yuv->V[k])*255;
		b=(yuv->Y[k]/255.+2.032*yuv->U[k])*255;
		if(r<0) r=0;
		if(g<0) g=0;
		if(b<0) b=0;
		if(r>255) r=255;
		if(g>255) g=255;
		if(b>255) b=255;
		MSEr+=pow(image->rdata[k]-round(r),2);
		MSEg+=pow(image->gdata[k]-round(g),2);
		MSEb+=pow(image->bdata[k]-round(b),2);
		image->rdata[k]=(unsigned char) round(r);
		image->gdata[k]=(unsigned char) round(g);
		image->bdata[k]=(unsigned char) round(b);
	}
    MSEr=MSEr/(image->pheader->pheight*image->pheader->pwidth);
	MSEg=MSEg/(image->pheader->pheight*image->pheader->pwidth);
	MSEb=MSEb/(image->pheader->pheight*image->pheader->pwidth);
    //Calculation of PSNR value for each channel
	yuv->psnr.rvalue=20*log10(255./sqrt(MSEr));
	yuv->psnr.gvalue=20*log10(255./sqrt(MSEg));
	yuv->psnr.bvalue=20*log10(255./sqrt(MSEb));
	return;
}
//YUV Equalization
void YUVEqualization(char *fname)
{
	char *newfile=(char *)malloc(30);
	struct ppm_file *resim=(ppm_file*)malloc(sizeof(ppm_file));
	if(resim==NULL)
	{
	    printf("\nMemory Problem!");exit(1);
	}
	int i;
	double average;
	struct YUV  *yuv=(struct YUV*)malloc(sizeof(struct YUV));
	if(yuv==NULL)
	{
		printf("\nMemory error");
		exit(1);
	}
	get_image_data(fname,resim);

	//Converts from RGB into YUV
	RGB2YUV(resim,yuv);
	//Histogram equalization on channel Y
	equalization(yuv->Y,resim->pheader->pwidth*resim->pheader->pheight);
	//Converts from YUV into RGB
	YUV2RGB(resim,yuv);
	average=(yuv->psnr.rvalue+yuv->psnr.gvalue+yuv->psnr.bvalue)/3.;
	printf("\n-------PSNR VALUES OF EACH CHANNEL ----------\n");
	printf("\nR Channel    :%lf",yuv->psnr.rvalue);
	printf("\nG Channel    :%lf",yuv->psnr.gvalue);
	printf("\nB Channel    :%lf",yuv->psnr.bvalue);
	printf("\nAverage Value:%lf",average); 
	//Histogram Equalized image
	i=0;
	while(fname[i]!='.') 
	{
		newfile[i]=fname[i];
		i++;
	}
	newfile[i]='\0';
	strcat(newfile,"_yuvequ.ppm");
	write_image(newfile,resim);
	free(yuv->Y);free(yuv->U);free(yuv->V);free(yuv);free(resim);
	system("pause");
	return;
}
