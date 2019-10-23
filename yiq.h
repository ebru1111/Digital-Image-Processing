void RGB2YIQ(struct ppm_file *image,struct YIQ *yiq); //Converts from RGB into YIQ
void YIQ2RGB(struct ppm_file *image,struct YIQ *yiq); //Converts from YIQ into RGB
void YIQEqualization(char *fname); //YIQ Equalization
//Converts from RGB into YIQ 
void RGB2YIQ(struct ppm_file *image,struct YIQ *yiq)
{
	int k,height,width;
	float r,g,b;
	height=image->pheader->pheight;
	width=image->pheader->pwidth;
    yiq->Y=(short int*)malloc(height*width*sizeof(short int));
    yiq->I=(float*)malloc(height*width*sizeof(float));
    yiq->Q=(float*)malloc(height*width*sizeof(float));
    if(yiq->Y==NULL || yiq->I==NULL || yiq->Q==NULL)
    {
    	printf("\nMemory error");
		exit(1);
	}
	for(k=0;k<height*width;k++)
	{
		r=image->rdata[k]/255.;
		g=image->gdata[k]/255.;
		b=image->bdata[k]/255.;
		yiq->Y[k]=round((0.299*r+0.587*g+0.114*b)*255);
		yiq->I[k]=0.595716*r-0.274453*g-0.321263*b;
		yiq->Q[k]=0.211456*r-0.522591*g+0.311135*b;
	}
	return;
	
}
//Converts from YIQ  into RGB
void YIQ2RGB(struct ppm_file *image,struct YIQ *yiq)
{
	int k;
	float r,g,b;
	double MSEr=0,MSEg=0,MSEb=0;
    for(k=0;k<image->pheader->pheight*image->pheader->pwidth;k++)
	{
		r=yiq->Y[k]/255.+0.9563*yiq->I[k]+0.6210*yiq->Q[k];
		g=yiq->Y[k]/255.-0.2721*yiq->I[k]-0.6474*yiq->Q[k];
		b=yiq->Y[k]/255.-1.1070*yiq->I[k]+1.7046*yiq->Q[k];;
		if(r<0) r=0;
		if(g<0) g=0;
		if(b<0) b=0;
		if(r>1) r=1;
		if(g>1) g=1;
		if(b>1) b=1;
		
		MSEr+=pow(image->rdata[k]-round(r*255),2);
		MSEg+=pow(image->gdata[k]-round(g*255),2);
		MSEb+=pow(image->bdata[k]-round(b*255),2);
		image->rdata[k]=(unsigned char) round(r*255);
		image->gdata[k]=(unsigned char) round(g*255);
		image->bdata[k]=(unsigned char) round(b*255);
	}
    MSEr=MSEr/(image->pheader->pheight*image->pheader->pwidth);
	MSEg=MSEg/(image->pheader->pheight*image->pheader->pwidth);
	MSEb=MSEb/(image->pheader->pheight*image->pheader->pwidth);
    //Calculation of PSNR value for each channel
	yiq->psnr.rvalue=20*log10(255./sqrt(MSEr));
	yiq->psnr.gvalue=20*log10(255./sqrt(MSEg));
	yiq->psnr.bvalue=20*log10(255./sqrt(MSEb));
	return;
}
//YIQ  Equalization
void YIQEqualization(char *fname)
{
	char *newfile=(char *)malloc(30);
	struct ppm_file *resim=(ppm_file*)malloc(sizeof(ppm_file));
	if(resim==NULL)
	{
	    printf("\nMemory Problem!");exit(1);
	}
	int i;
	double average;
	struct YIQ  *yiq=(struct YIQ*)malloc(sizeof(struct YIQ));
	if(yiq==NULL)
	{
		printf("\nMemory error");
		exit(1);
	}
	get_image_data(fname,resim);

	//Converts from RGB into YIQ 
	RGB2YIQ(resim,yiq);
	//Histogram equalization on channel Y
	equalization(yiq->Y,resim->pheader->pwidth*resim->pheader->pheight);
	//Converts from YIQ  into RGB
	YIQ2RGB(resim,yiq);
	average=(yiq->psnr.rvalue+yiq->psnr.gvalue+yiq->psnr.bvalue)/3.;
	printf("\n-------PSNR VALUES OF EACH CHANNEL ----------\n");
	printf("\nR Channel    :%lf",yiq->psnr.rvalue);
	printf("\nG Channel    :%lf",yiq->psnr.gvalue);
	printf("\nB Channel    :%lf",yiq->psnr.bvalue);
	printf("\nAverage Value:%lf",average); 
	//Histogram Equalized image
	i=0;
	while(fname[i]!='.') 
	{
		newfile[i]=fname[i];
		i++;
	}
	newfile[i]='\0';
	strcat(newfile,"_yiqequ.ppm");
	write_image(newfile,resim);
	free(yiq->Y);free(yiq->I);free(yiq->Q);free(yiq);free(resim);
	system("pause");
	return;
}
