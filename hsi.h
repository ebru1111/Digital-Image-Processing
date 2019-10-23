void RGB2HSI(struct ppm_file *image,struct HSI *hsi); //Converts RGB into HSI Channels
void HSI2RGB(struct ppm_file *image,struct HSI *hsi); //Converts from HSI to RGB channels
void HSIEqualization(char *fname); //HSI Equalizaion function
//Converts from RGB into HSI Channels
void RGB2HSI(struct ppm_file *image,struct HSI *hsi)
{
	int k,height,width;
	float r,g,b,i,s,h;
	height=image->pheader->pheight;
	width=image->pheader->pwidth;
    hsi->H=(short int*)malloc(height*width*sizeof(short int));
    hsi->S=(short int*)malloc(height*width*sizeof(short int));
    hsi->I=(short int*)malloc(height*width*sizeof(short int));
    if(hsi->H==NULL || hsi->S==NULL || hsi->I==NULL)
    {
    	printf("\nMemory error");
		exit(1);
	}
	for(k=0;k<height*width;k++)
	{
		r=(float)(image->rdata[k])/(image->rdata[k]+image->gdata[k]+image->bdata[k]);
		g=(float)(image->gdata[k])/(image->rdata[k]+image->gdata[k]+image->bdata[k]);
		b=(float)(image->bdata[k])/(image->rdata[k]+image->gdata[k]+image->bdata[k]);
		if(b<=g) h=acos((0.5*(2*r-g-b))/sqrt(pow(r-g,2)+(r-b)*(g-b)));
		else h=2*PI-acos((0.5*(2*r-g-b))/sqrt(pow(r-g,2)+(r-b)*(g-b)));
		s=1-3*min(r,min(g,b));
		hsi->H[k]= (h*180/PI);
		hsi->S[k]= (s*100);
		hsi->I[k]= (image->rdata[k]+image->gdata[k]+image->bdata[k])/3.;
	}
	return;
	
}
//Converts from HSI into RGB channels
void HSI2RGB(struct ppm_file *image,struct HSI *hsi)
{
	int k;
	float r,g,b,i,s,h,x,y,z;
	double MSEr=0,MSEg=0,MSEb=0;
	unsigned char R,G,B;
    for(k=0;k<image->pheader->pheight*image->pheader->pwidth;k++)
	{
		h=hsi->H[k]*PI/180.;
		s=hsi->S[k]/100.;
		i=hsi->I[k]/255.;
		x=i*(1-s);
		if(h<2*PI/3)
		{
			y=i*(1+s*cos(h)/cos(PI/3-h));
			z=3*i-(x+y);
			b=x;r=y;g=z;
		}
		else if(h<4*PI/3)
		{
			h-=2*PI/3;
			y=i*(1+s*cos(h)/cos(PI/3-h));
			z=3*i-(x+y);
			r=x;g=y;b=z;
		}
		else 
		{
			h-=4*PI/3;
			y=i*(1+s*cos(h)/cos(PI/3-h));
			z=3*i-(x+y);
			r=z;g=x;b=y;
		}
		if(r>1) r=1;
		if(g>1) g=1;
		if(b>1) b=1;
		R=(unsigned char)(r*255);
		G=(unsigned char)(g*255);
		B=(unsigned char)(b*255);
		// Calculation of Mean Square Error for each channel
		MSEr+=pow(image->rdata[k]-R,2);
		MSEg+=pow(image->gdata[k]-G,2);
		MSEb+=pow(image->bdata[k]-B,2);
		//Obtaining of equalized image for each channel
		image->rdata[k]=R;
		image->gdata[k]=G;
		image->bdata[k]=B;
	}
	MSEr=MSEr/(image->pheader->pheight*image->pheader->pwidth);
	MSEg=MSEg/(image->pheader->pheight*image->pheader->pwidth);
	MSEb=MSEb/(image->pheader->pheight*image->pheader->pwidth);
    //Calculation of PSNR value for each channel
	hsi->psnr.rvalue=20*log10(255./sqrt(MSEr));
	hsi->psnr.gvalue=20*log10(255./sqrt(MSEg));
	hsi->psnr.bvalue=20*log10(255./sqrt(MSEb));
	return;
}
//HSI Equalizaion function
void HSIEqualization(char *fname)
{
	char *newfile=(char *)malloc(30);
	struct ppm_file *resim=(ppm_file*)malloc(sizeof(ppm_file));
	if(resim==NULL)
	{
	    printf("\nMemory Problem!");exit(1);
	}
	int i;
	double average;
	struct HSI  *hsi=(struct HSI*)malloc(sizeof(struct HSI));
	if(hsi==NULL)
	{
		printf("\nMemory error");
		exit(1);
	}
	get_image_data(fname,resim);
	//Converts from RGB to HSI
	RGB2HSI(resim,hsi);
	//Histogram equalization on channel I
	equalization(hsi->I,resim->pheader->pwidth*resim->pheader->pheight);
	//Converts from HSI to RGB
	HSI2RGB(resim,hsi);
	average=(hsi->psnr.rvalue+hsi->psnr.gvalue+hsi->psnr.bvalue)/3.;
	printf("\n-------PSNR VALUES OF EACH CHANNEL ----------\n");
	printf("\nR Channel    :%lf",hsi->psnr.rvalue);
	printf("\nG Channel    :%lf",hsi->psnr.gvalue);
	printf("\nB Channel    :%lf",hsi->psnr.bvalue);
	printf("\nAverage Value:%lf",average); 
	//Histogram Equalized image
	i=0;
	while(fname[i]!='.') 
	{
		newfile[i]=fname[i];
		i++;
	}
	newfile[i]='\0';
	strcat(newfile,"_hsiequ.ppm");
	write_image(newfile,resim);
	free(hsi->H);free(hsi->S);free(hsi->I);free(hsi);free(resim);
	system("pause");
	return;
}
