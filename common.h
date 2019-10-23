void histogram(float hist[],short int *channel,int n); //Calculation of Histogram of image
void equalization(short int *channel,int n); //Histogram Equalization function
void menu(); //Menu function
float min(float a,float b); //returns min value of two float variables
float min(float a,float b); //returns max value of two float variables
//Calculation of Histogram of image
void histogram(float hist[],short int *channel,int n)
{
	int i;
	for(i=0;i<256;i++)hist[i]=0;
	for(i=0;i<n;i++) hist[(int)channel[i]]++;
	return;
}
//Histogram Equalization function
void equalization(short int *channel,int n)
{
	float t=0;
	int i;
	float hist[256];
	histogram(hist,channel,n);
	//Histogram equalization normalization 
	for(i=0;i<256;i++)
	{
		t+=hist[i];
		hist[i]=t;
		hist[i]=round(255*hist[i]/n);
	}
	//obtaning new volues of channel
	for(i=0;i<n;i++) channel[i]= hist[(int)channel[i]];
	
}
//returns min value of two float variables
float min(float a,float b)
{
	if(a<b)return a;
	return b;
}
float max(float a,float b)
{
	if(a>b)return a;
	return b;
}
void menu()
{
	printf("\t----------- M E N U -----------------");
	printf("\n\t1.HSI Histogram Equalization\n");
	printf("\t2.YCbCr (SDTV) Histogram Equalization\n");
	printf("\t3.YCbCr (HDTV) Histogram Equalization\n");
	printf("\t4.HSV Histogram Equalization\n");
	printf("\t5.HSL Histogram Equalization\n");
	printf("\t6.YUV Histogram Equalization\n");
	printf("\t7.YPbPr (SDTV) Histogram Equalization\n");
	printf("\t8.YPbPr (HDTV) Histogram Equalization\n");
	printf("\t9.YIQ Histogram Equalization\n");
	printf("\t10.MCYK Histogram Equalization\n");
	printf("\t11.END\n");
	return;
}
