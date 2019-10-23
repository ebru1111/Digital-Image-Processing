void RGB2HSL(struct ppm_file *image,struct HSL *hsl); //Converts from RGB into HSL
void HSL2RGB(struct ppm_file *image,struct HSL *hsl); //Converts from HSL into RGB
void HSLEqualization(char *fname); //HSL Equalization
//Converts from RGB into HSL
void RGB2HSL(struct ppm_file *image,struct HSL *hsl)
{
	int k;
	float r,g,b,cmax,cmin,delta;
    hsl->H=(float*)malloc(image->pheader->pheight*image->pheader->pwidth*sizeof(float));
    hsl->S=(float*)malloc(image->pheader->pheight*image->pheader->pwidth*sizeof(float));
    hsl->L=(short int*)malloc(image->pheader->pheight*image->pheader->pwidth*sizeof(short int));
    if(hsl->H==NULL || hsl->S==NULL || hsl->L==NULL)
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
		hsl->L[k]=255*(cmax+cmin)/2;
		if(delta==0) hsl->H[k]=0;
		else if(cmax==r) hsl->H[k]=60*fmod((g-b)/delta,6);
		else if(cmax==g) hsl->H[k]=60*((b-r)/delta+2);
		else if(b==cmax)hsl->H[k]=60*((r-g)/delta+4);
		if(delta==0)hsl->S[k]=0;
		else hsl->S[k]=delta/(1-fabs(2*hsl->L[k]/255.-1));
	}
	return;
	
}
//Converts from HSL into RGB
void HSL2RGB(struct ppm_file *image,struct HSL *hsl)
{
	int k;
    double c,x,m,r,g,b;
	double MSEr=0,MSEg=0,MSEb=0;
	short int R,G,B;
    for(k=0;k<image->pheader->pheight*image->pheader->pwidth;k++)
	{
		c=(1-fabs(2*hsl->L[k]/255.-1))*hsl->S[k];
		x=c*(1-fabs(fmod(hsl->H[k]/60,2)-1));
		if(x<0) x=0;
		m=hsl->L[k]/255.-c/2.;
		if(m<0)m=0;//printf("\n%f",m);
		if(hsl->H[k]<60)
		{
			r=c;g=x;b=0;
		}
		else if(hsl->H[k]<120)
		{
			r=x;g=c;b=0;
		}
		else if(hsl->H[k]<180)
		{
			r=0;g=c;b=x;
		}
		else if(hsl->H[k]<240)
		{
			r=0;g=x;b=c;
		}
		else if(hsl->H[k]<300)
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
		MSEr+=pow(image->rdata[k]-R,2);
		MSEg+=pow(image->gdata[k]-G,2);
		MSEb+=pow(image->bdata[k]-B,2);
		image->rdata[k]=(unsigned char)R;
		image->gdata[k]=(unsigned char)G;
		image->bdata[k]=(unsigned char)B;
	}
	
    MSEr=MSEr/(image->pheader->pheight*image->pheader->pwidth);
	MSEg=MSEg/(image->pheader->pheight*image->pheader->pwidth);
	MSEb=MSEb/(image->pheader->pheight*image->pheader->pwidth);
	
    //Calculation of PSNR value for each channel
	hsl->psnr.rvalue=20*log10(255./sqrt(MSEr));
	hsl->psnr.gvalue=20*log10(255./sqrt(MSEg));
	hsl->psnr.bvalue=20*log10(255./sqrt(MSEb));
	return;
}
//YCbCr Equalization
void HSLEqualization(char *fname)
{
	char *newfile=(char *)malloc(30);
	struct ppm_file *resim=(ppm_file*)malloc(sizeof(ppm_file));
	if(resim==NULL)
	{
	    printf("\nMemory Problem!");exit(1);
	}
	int i;
	double average;
	struct HSL  *hsl=(struct HSL*)malloc(sizeof(struct HSL));
	if(hsl==NULL)
	{
		printf("\nMemory error");
		exit(1);
	}
	get_image_data(fname,resim);
	//Converts from RGB into HSL
	RGB2HSL(resim,hsl);
	//Histogram equalization on channel L
	equalization(hsl->L,resim->pheader->pwidth*resim->pheader->pheight);
	//Converts from HSL into RGB
	HSL2RGB(resim,hsl);
	average=(hsl->psnr.rvalue+hsl->psnr.gvalue+hsl->psnr.bvalue)/3.;
	printf("\n-------PSNR VALUES OF EACH CHANNEL ----------\n");
	printf("\nR Channel    :%lf",hsl->psnr.rvalue);
	printf("\nG Channel    :%lf",hsl->psnr.gvalue);
	printf("\nB Channel    :%lf",hsl->psnr.bvalue);
	printf("\nAverage Value:%lf",average); 
	//Histogram Equalized image
	i=0;
	while(fname[i]!='.') 
	{
		newfile[i]=fname[i];
		i++;
	}
	newfile[i]='\0';
	strcat(newfile,"_hslequ.ppm");
	write_image(newfile,resim);
	free(hsl->H);free(hsl->S);free(hsl->L);free(hsl);free(resim);
	system("pause");
	return;
}
