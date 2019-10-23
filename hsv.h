void RGB2HSV(struct ppm_file *image,struct HSV *hsv); //Converts from RGB into HSV
void HSV2RGB(struct ppm_file *image,struct HSV *hsv); //Converts from HSV into RGB
void HSVEqualization(char *fname); //HSV Equalization
//Converts from RGB into HSV
void RGB2HSV(struct ppm_file *image,struct HSV *hsv)
{
	int k;
	float r,g,b,cmax,cmin,delta;
    hsv->H=(float*)malloc(image->pheader->pheight*image->pheader->pwidth*sizeof(float));
    hsv->S=(float*)malloc(image->pheader->pheight*image->pheader->pwidth*sizeof(float));
    hsv->V=(short int*)malloc(image->pheader->pheight*image->pheader->pwidth*sizeof(short int));
    if(hsv->H==NULL || hsv->S==NULL || hsv->V==NULL)
    {
    	printf("\nMemory error");
		exit(1);
	}
	for(k=0;k<image->pheader->pheight*image->pheader->pwidth;k++)
	{
		r=image->rdata[k]/255.;
		g=image->gdata[k]/255.;
		b=image->bdata[k]/255.;
		cmax=max(r,max(g,b));
		cmin=min(r,min(g,b));
		delta=cmax-cmin;
		if(delta==0) hsv->H[k]=0;
		else if(cmax==r) hsv->H[k]=60*fmod((g-b)/delta,6);
		else if(cmax==g) hsv->H[k]=60*((b-r)/delta+2);
		else hsv->H[k]=60*((r-g)/delta+4);
		hsv->V[k]=cmax*255;  
		if(cmax==0) hsv->S[k]=0;
		else hsv->S[k]=delta/cmax;
	}
	return;
	
}
//Converts from HSV into RGB
void HSV2RGB(struct ppm_file *image,struct HSV *hsv)
{
	int k;
    double c,x,m,r,g,b;
	double MSEr=0,MSEg=0,MSEb=0;
	short int R,G,B;
    for(k=0;k<image->pheader->pheight*image->pheader->pwidth;k++)
	{
		c=hsv->V[k]/255.*hsv->S[k];
		x=c*(1-fabs(fmod(hsv->H[k]/60.,2)-1));
		m=hsv->V[k]/255.-c;
		if(x<0) x=0;//printf("\nx=%lf",x);
		if(hsv->H[k]<60)
		{
			r=c;g=x;b=0;
		}
		else if(hsv->H[k]<120)
		{
			r=x;g=c;b=0;
		}
		else if(hsv->H[k]<180)
		{
			r=0;g=c;b=x;
		}
		else if(hsv->H[k]<240)
		{
			r=0;g=x;b=c;
		}
		else if(hsv->H[k]<300)
		{
			r=x;g=0;b=c;
		}
		else 
		{
			r=c;g=0;b=x;
		}
        R=(r+m)*255;
        G=(g+m)*255;
        B=(b+m)*255;
        if(R>255)R=255;
        if(G>255)G=255;
        if(B>255)B=255;
		MSEr+=pow(image->rdata[k]-R,2);
		MSEg+=pow(image->gdata[k]-G,2);
		MSEb+=pow(image->bdata[k]-B,2);
		image->rdata[k]=(unsigned char) R;
		image->gdata[k]=(unsigned char) G;
		image->bdata[k]=(unsigned char) B;
	}
    MSEr=MSEr/(image->pheader->pheight*image->pheader->pwidth);
	MSEg=MSEg/(image->pheader->pheight*image->pheader->pwidth);
	MSEb=MSEb/(image->pheader->pheight*image->pheader->pwidth);
    //Calculation of PSNR value for each channel
	hsv->psnr.rvalue=20*log10(255./sqrt(MSEr));
	hsv->psnr.gvalue=20*log10(255./sqrt(MSEg));
	hsv->psnr.bvalue=20*log10(255./sqrt(MSEb));
	return;
}
//YCbCr Equalization
void HSVEqualization(char *fname)
{
	char *newfile=(char *)malloc(30);
	struct ppm_file *resim=(ppm_file*)malloc(sizeof(ppm_file));
	if(resim==NULL)
	{
	    printf("\nMemory Problem!");exit(1);
	}
	int i;
	double average;
	struct HSV  *hsv=(struct HSV*)malloc(sizeof(struct HSV));
	if(hsv==NULL)
	{
		printf("\nMemory error");
		exit(1);
	}
	get_image_data(fname,resim);
	//Converts from RGB into HSV
	RGB2HSV(resim,hsv);
	//Histogram equalization on channel V
	equalization(hsv->V,resim->pheader->pwidth*resim->pheader->pheight);
	//Converts from HSV into RGB
	HSV2RGB(resim,hsv);
	average=(hsv->psnr.rvalue+hsv->psnr.gvalue+hsv->psnr.bvalue)/3.;
	printf("\n-------PSNR VALUES OF EACH CHANNEL ----------\n");
	printf("\nR Channel    :%lf",hsv->psnr.rvalue);
	printf("\nG Channel    :%lf",hsv->psnr.gvalue);
	printf("\nB Channel    :%lf",hsv->psnr.bvalue);
	printf("\nAverage Value:%lf",average); 
	//Histogram Equalized image
	i=0;
	while(fname[i]!='.') 
	{
		newfile[i]=fname[i];
		i++;
	}
	newfile[i]='\0';
	strcat(newfile,"_hsvequ.ppm");
	write_image(newfile,resim);
	free(hsv->H);free(hsv->S);free(hsv->V);free(hsv);free(resim);
	system("pause");
	return;
}

