#pragma pack(1)
// struct for ppm header
struct ppm_header
{
	char pgmtype1;
	char pgmtype2;
	int pwidth;
	int pheight;
	int pmax;
};
//struct for ppm file 
struct ppm_file
{
	struct ppm_header *pheader;
	unsigned char *rdata,*gdata,*bdata;
};
//struct for PSNR Calculation of each channel
struct Psnr
{
	double rvalue;
	double gvalue;
	double bvalue;
};
//struct for HSI channels
struct HSI
{
	short int *H;
	short int *S;
	short int *I;
	struct Psnr psnr;
};
//struct for YCbCr channels (SDTV) and (HDTV)
struct YCbCr
{
	short int *Y;
	short int *Cb;
	short int *Cr;
	struct Psnr psnr;
};
//struct for HSV channels
struct HSV
{
	float *H;
	float *S;
	short int *V;
	struct Psnr psnr;	
};
//struct for HSL channels
struct HSL
{
	float *H;
	float *S;
	short int *L;
	struct Psnr psnr;	
};
//struct for YUV channels
struct YUV
{
	short int *Y;
	float *U;
	float *V;
	struct Psnr psnr;		
};
//struct for YPbPr channels (HDTV) and (SDTV)
struct YPbPr
{
	short int *Y;
	float *Pb;
	float *Pr;
	struct Psnr psnr;		
};
//struct for YIQ channels 
struct YIQ
{
	short int *Y;
	float *I;
	float *Q;
	struct Psnr psnr;		
};
//struct for CMYK channels 
struct CMYK
{
	short int *C;
	short int *M;
	short int *Y;
	short int *K;
	struct Psnr psnr;		
};
