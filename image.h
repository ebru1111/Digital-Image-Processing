void get_image_data(char *filename,struct ppm_file *image); //read image function
void write_image(char *filename,struct ppm_file *image); //write image function
//read image function
void get_image_data(char *filename, struct ppm_file *image ) 
{
	FILE* fp;
	int i=0;
	char temp[256];
	image->pheader=(struct ppm_header *)malloc(sizeof(struct ppm_header));
	fp = fopen(filename, "rb" );
	if (fp==NULL) 
	{
		printf("Dosya acilamadi: %s.\n\n", filename);
		exit(1);
	}
	printf ("Okunan PPM dosyasi : %s...\n", filename);
	fscanf (fp, "%s", temp);
	if (strcmp(temp, "P6") == 0) 
	{
		image->pheader->pgmtype1=temp[0];
		image->pheader->pgmtype2=temp[1];
		fscanf (fp, "%s", temp);
		if (temp[0]=='#') 
		{
			while(fgetc(fp)!='\n');
			fscanf (fp, "%d %d\n",&image->pheader->pwidth,&image->pheader->pheight);
			fscanf (fp, "%d\n", &image->pheader->pmax);
		}
		else
		{
			sscanf (temp, "%d", &image->pheader->pwidth);
			fscanf (fp, "%d\n", &image->pheader->pheight);
			fscanf (fp, "%d\n", &image->pheader->pmax);
		}
		image->rdata=(unsigned char *)malloc(image->pheader->pheight*image->pheader->pwidth*sizeof(unsigned char));
		image->gdata=(unsigned char *)malloc(image->pheader->pheight*image->pheader->pwidth*sizeof(unsigned char));
		image->bdata=(unsigned char *)malloc(image->pheader->pheight*image->pheader->pwidth*sizeof(unsigned char));
		if (image->rdata==NULL) printf("bellek problemi...\n");
		for(i=0;i<image->pheader->pwidth*image->pheader->pheight;i++)
		{
			fread(&image->rdata[i],1,1,fp);
			fread(&image->gdata[i],1,1,fp);
			fread(&image->bdata[i],1,1,fp);
		}
	}
	else 
	{
		printf ("\nHata Resim dosyasi PGM P6 formatinda degil");
		exit(1);
	}
	fclose(fp);
}
//write image function
void write_image(char *filename,struct ppm_file *image)
{
	FILE *fp;
	int i,max=0;
	fp=fopen(filename,"wb");
	fputc(image->pheader->pgmtype1,fp);
	fputc(image->pheader->pgmtype2,fp);
	fputc('\n',fp);
	fprintf(fp,"%d %d\n",image->pheader->pwidth,image->pheader->pheight);
	fprintf(fp,"%d\n",255/*max*/);
	printf("\n\n----------------------------------\n\n");
	for(i=0;i<image->pheader->pwidth*image->pheader->pheight;i++)
	{
		fwrite(&image->rdata[i],1,1,fp);
		fwrite(&image->gdata[i],1,1,fp);
		fwrite(&image->bdata[i],1,1,fp);
	}
	fclose(fp);
}
