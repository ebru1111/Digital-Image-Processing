void RGB2YPbPr(struct ppm_file *image,struct YPbPr *ypbpr); //Converts from RGB into YPbPr
void YPbPr2RGB(struct ppm_file *image,struct YPbPr *ypbpr); //Converts from YPbPr into RGB
void YPbPrEqualization(char *fname); //YPbPr Equalization
//Converts from RGB into YUV
void RGB2YPbPr(struct ppm_file *image,struct YPbPr *ypbpr)
{
	int k,height,width;
	float r,g,b;
	height=image->pheader->pheight;
	width=image->pheader->pwidth;
    ypbpr->Y=(short int*)malloc(height*width*sizeof(short int));
    ypbpr->Pb=(float*)malloc(height*width*sizeof(float));
    ypbpr->Pr=(float*)malloc(height*width*sizeof(float));
    if(ypbpr->Y==NULL || ypbpr->Pb==NULL || ypbpr->Pr==NULL)
    {
    	printf("\nMemory error");
		exit(1);
	}
	for(k=0;k<height*width;k++)
	{
		r=image->rdata[k];
		g=image->gdata[k];
		b=image->bdata[k];
		ypbpr->Y[k]=round(0.299*r+0.587*g+0.114*b);
		ypbpr->Pb[k]=-0.169*r-0.331*g+0.5*b;
		ypbpr->Pr[k]=0.5*r-0.419*g-0.081*b;
	}
	return;
	
}
//Converts from YUV into RGB
void YPbPr2RGB(struct ppm_file *image,struct YPbPr *ypbpr)
{
	int k;
	float r,g,b;
	double MSEr=0,MSEg=0,MSEb=0;
    for(k=0;k<image->pheader->pheight*image->pheader->pwidth;k++)
	{
		r=ypbpr->Y[k]+1.14*ypbpr->Pr[k];
		g=ypbpr->Y[k]-0.344*ypbpr->Pb[k]-0.714*ypbpr->Pr[k];
		b=ypbpr->Y[k]+1.772*ypbpr->Pb[k];
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
	ypbpr->psnr.rvalue=20*log10(255./sqrt(MSEr));
	ypbpr->psnr.gvalue=20*log10(255./sqrt(MSEg));
	ypbpr->psnr.bvalue=20*log10(255./sqrt(MSEb));
	return;
}
//YUV Equalization
void YPbPrEqualization(char *fname)
{
	char *newfile=(char *)malloc(30);
	struct ppm_file *resim=(ppm_file*)malloc(sizeof(ppm_file));
	if(resim==NULL)
	{
	    printf("\nMemory Problem!");exit(1);
	}
	int i;
	double average;
	struct YPbPr  *ypbpr=(struct YPbPr*)malloc(sizeof(struct YPbPr));
	if(ypbpr==NULL)
	{
		printf("\nMemory error");
		exit(1);
	}
	get_image_data(fname,resim);

	//Converts from RGB into YUV
	RGB2YPbPr(resim,ypbpr);
	//Histogram equalization on channel Y
	equalization(ypbpr->Y,resim->pheader->pwidth*resim->pheader->pheight);
	//Converts from YUV into RGB
	YPbPr2RGB(resim,ypbpr);
	average=(ypbpr->psnr.rvalue+ypbpr->psnr.gvalue+ypbpr->psnr.bvalue)/3.;
	printf("\n-------PSNR VALUES OF EACH CHANNEL ----------\n");
	printf("\nR Channel    :%lf",ypbpr->psnr.rvalue);
	printf("\nG Channel    :%lf",ypbpr->psnr.gvalue);
	printf("\nB Channel    :%lf",ypbpr->psnr.bvalue);
	printf("\nAverage Value:%lf",average); 
	//Histogram Equalized image
	i=0;
	while(fname[i]!='.') 
	{
		newfile[i]=fname[i];
		i++;
	}
	newfile[i]='\0';
	strcat(newfile,"_ypbprequ.ppm");
	write_image(newfile,resim);
	free(ypbpr->Y);free(ypbpr->Pb);free(ypbpr->Pr);free(ypbpr);free(resim);
	system("pause");
	return;
}
