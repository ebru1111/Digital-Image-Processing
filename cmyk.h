void RGB2CMYK(struct ppm_file *image,struct CMYK *cmyk); //Converts from RGB into CMYK
void CMYK2RGB(struct ppm_file *image,struct CMYK *cmyk); //Converts from CMYK into RGB
void CMYKEqualization(char *fname); //CMYK Equalization
//Converts from RGB into CMYK 
void RGB2CMYK(struct ppm_file *image,struct CMYK *cmyk)
{
	int k,height,width;
	float r,g,b;
	height=image->pheader->pheight;
	width=image->pheader->pwidth;
    cmyk->C=(short int*)malloc(height*width*sizeof(short int));
    cmyk->M=(short int*)malloc(height*width*sizeof(short int));
    cmyk->Y=(short int*)malloc(height*width*sizeof(short int));
    cmyk->K=(short int*)malloc(height*width*sizeof(short int));
    if(cmyk->C==NULL || cmyk->M==NULL || cmyk->Y==NULL || cmyk->K==NULL)
    {
    	printf("\nMemory error");
		exit(1);
	}
	for(k=0;k<height*width;k++)
	{
		r=image->rdata[k]/255.;
		g=image->gdata[k]/255.;
		b=image->bdata[k]/255.;
		cmyk->K[k]=(1-max(r,max(g,b)))*255;
		cmyk->C[k]=(1-r-cmyk->K[k]/255.)*255./(1-cmyk->K[k]/255.);
		cmyk->M[k]=(1-g-cmyk->K[k]/255.)*255./(1-cmyk->K[k]/255.);
		cmyk->Y[k]=(1-b-cmyk->K[k]/255.)*255./(1-cmyk->K[k]/255.);
	}
	return;
	
}
//Converts from CMYK  into RGB
void CMYK2RGB(struct ppm_file *image,struct CMYK *cmyk)
{
	int k;
	short int r,g,b;
	double MSEr=0,MSEg=0,MSEb=0;
    for(k=0;k<image->pheader->pheight*image->pheader->pwidth;k++)
	{
		r=255*(1-cmyk->C[k]/255.)*(1-cmyk->K[k]/255.);
		g=255*(1-cmyk->M[k]/255.)*(1-cmyk->K[k]/255.);
		b=255*(1-cmyk->Y[k]/255.)*(1-cmyk->K[k]/255.);
	
		
		MSEr+=pow(image->rdata[k]-r,2);
		MSEg+=pow(image->gdata[k]-g,2);
		MSEb+=pow(image->bdata[k]-b,2);
		image->rdata[k]=(unsigned char) r;
		image->gdata[k]=(unsigned char) g;
		image->bdata[k]=(unsigned char) b;
	}
    MSEr=MSEr/(image->pheader->pheight*image->pheader->pwidth);
	MSEg=MSEg/(image->pheader->pheight*image->pheader->pwidth);
	MSEb=MSEb/(image->pheader->pheight*image->pheader->pwidth);
    //Calculation of PSNR value for each channel
	cmyk->psnr.rvalue=20*log10(255./sqrt(MSEr));
	cmyk->psnr.gvalue=20*log10(255./sqrt(MSEg));
	cmyk->psnr.bvalue=20*log10(255./sqrt(MSEb));
	return;
}
//CMYK  Equalization
void CMYKEqualization(char *fname)
{
	char *newfile=(char *)malloc(30);
	struct ppm_file *resim=(ppm_file*)malloc(sizeof(ppm_file));
	if(resim==NULL)
	{
	    printf("\nMemory Problem!");exit(1);
	}
	int i;
	double average;
	struct CMYK  *cmyk=(struct CMYK*)malloc(sizeof(struct CMYK));
	if(cmyk==NULL)
	{
		printf("\nMemory error");
		exit(1);
	}
	get_image_data(fname,resim);

	//Converts from RGB into CMYK 
	RGB2CMYK(resim,cmyk);
	//Histogram equalization on channel Y
	equalization(cmyk->K,resim->pheader->pwidth*resim->pheader->pheight);
	//Converts from CMYK  into RGB
	CMYK2RGB(resim,cmyk);
	average=(cmyk->psnr.rvalue+cmyk->psnr.gvalue+cmyk->psnr.bvalue)/3.;
	printf("\n-------PSNR VALUES OF EACH CHANNEL ----------\n");
	printf("\nR Channel    :%lf",cmyk->psnr.rvalue);
	printf("\nG Channel    :%lf",cmyk->psnr.gvalue);
	printf("\nB Channel    :%lf",cmyk->psnr.bvalue);
	printf("\nAverage Value:%lf",average); 
	//Histogram Equalized image
	i=0;
	while(fname[i]!='.') 
	{
		newfile[i]=fname[i];
		i++;
	}
	newfile[i]='\0';
	strcat(newfile,"_cmykequ.ppm");
	write_image(newfile,resim);
	free(cmyk->C);free(cmyk->M);free(cmyk->Y);free(cmyk->K);free(cmyk);free(resim);
	system("pause");
	return;
}
