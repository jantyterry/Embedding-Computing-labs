#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int Compress(unsigned char ext[4]);
int Decompress();

FILE *fpt;
FILE *fpt2;
char input[80]; 


int main(int argc,char *argv[])
{	int i, j;
	unsigned char ext[4];
	char t_input[80];
	
	
	strcpy(t_input,argv[2]);
	if((fpt=fopen(t_input,"rb")) == NULL)
	{
		printf("Can't open file for reading.\n");
		exit(0);
	}
	for(i=0;i<strlen(argv[2]);i++)
	{
		if(t_input[i]=='.')
		   break;

		else input[i]=t_input[i];
	}
	
	for(j=0;j<3;j++)
	   ext[j]=t_input[j+i+1];

	if(strcmp(argv[1],"-c")==0)
	{
	       Compress(ext);
	    
	}
	
	if(strcmp(argv[1],"-u")==0){

		Decompress();
	    
	}
	fclose(fpt);
	return(0);
}

int Compress(unsigned char ext[4])
{
	unsigned char run_count; 
	unsigned char A,B; 
	char output[80]; 
	unsigned char marker = 0;
	unsigned char max = 255;
	
	
	
	strcpy(output,input);
	strcat(output,".rle");
	if((fpt2 =fopen(output,"wb"))==NULL)
	{
	    printf("Can't create output file.\n");
	    exit(0);
	}
	fputs(ext,fpt2);
	fread(&A,1,1,fpt);
	run_count=1;

	while(feof(fpt) == 0)
	{
		if(fread(&B,1,1,fpt)==0)
		   break;

		

		if((A==B) && (run_count < max)){
			run_count++;
			
		}
		
		else if (run_count > 1){

			fputc(marker,fpt2);
			fputc(run_count,fpt2);
			fputc(A,fpt2);
			A=B;
			run_count =1;
			

		}
		
		else{
			if((A!=B) && (run_count < max)){
	
			fputc(A,fpt2);
			A=B;
			run_count=1;
			}
		
		}

		
	}
	
	fputc(marker,fpt2);
	fputc(run_count,fpt2);
	fputc(A,fpt2);
	printf("Compression Complete!!!!!\n");
	fclose(fpt2);
	return(0);
}

int Decompress()
{
	char output[80];
	unsigned char run_count;
	unsigned char A;
	unsigned char *ptr;
	int i;
	unsigned char marking;
	
	
	strcpy(output,input);
	strcat(output,".urle.");
	ptr = (unsigned char *)calloc(1,1);
	
	
	for(i=0;i<3;i++)
	{
	   fread(ptr,1,1,fpt);
	   strcat(output,ptr);
	}
	if((fpt2=fopen(output,"wb"))==NULL)
	{
		printf("Can't create output file.\n");
		return(0);
	}
	while(feof(fpt) == 0)
	{

		if(fread(&marking,1,1,fpt)==0)
		   break;
		
		if(marking == 0){

		fread(&run_count,1,1,fpt);
		fread(&A,1,1,fpt);
		for(i=0;i<run_count;i++)
		    fputc(A,fpt2);

		}

		
		else fputc(marking,fpt2);


	}
	printf("Decompression Complete!!!!!!\n");
	free(ptr);
	fclose(fpt2);
	return(0);
}



