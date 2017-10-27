#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int Compress(unsigned char ext[3]);
int Decompress();
int Usage();

FILE *fpt;
FILE *fpt2;
char input[80]; 


int main(int argc,char *argv[])
{	int i, j;
	unsigned char ext[3];
	char t_input[80];

	if(argc != 3){

	   Usage();
	   return(0);
	}
	
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




int Usage(){
	printf("Usage:\nSpecify the file to be compressed or decompressed...\nlzw -c input  #compress file input\nlzw -u input  #decompress file input\nCompressed file will be found in the parent folder with the same name as the file but with a .lzw extension\nDecompressed file will be found in the parent folder with the same name as the file but with a .ulzw. extension\n");

return(0);
}





int Compress(unsigned char ext[3])
{
	unsigned char d[65535][100];
    	unsigned char d_length[65535]; 
	unsigned short int code;
	char output[80];
    	unsigned char c;
    	unsigned char p[100],p_c[101]; 
    	unsigned short int range = 256; 
    	unsigned short int i;
	unsigned short int len_p = 0; 
	

        strcpy(output,input);
        strcat(output,".lzw");
        if((fpt2=fopen(output,"wb"))==NULL){

            printf("Can't create output file.\n");
	    exit(0);
        }
        fputs(ext,fpt2);
	
	for(i=0;i<range;i++){

	    sprintf(d[i],"%c",i);
	    d_length[i] = 1;
	}
	
	printf("File is compressing......\n");

	while(feof(fpt) == 0){

	    if(fread(&c,1,1,fpt)==0)
		break;

            p_c[len_p] = c;
            code = range;

            for(i=0;((i<range) && (code==range)); i++){

               if(memcmp(d[i],p_c,(len_p + 1)) == 0)
                 code = i;
            }

            if((code < range) && (len_p < 99)){

        	p[len_p] = c;
        	len_p++;
       	    }

            else{

        	code = range;
                for(i=0;((i<range) && (code==range)); i++){

                   if(memcmp(d[i],p,len_p) == 0)
                     code = i;
                }

                if(fwrite(&code,2,1,fpt2)==0)
		   break;

		if(range<65535){

	           memcpy(d[range],p_c,(len_p + 1));
	           d_length[range] = (len_p + 1);
	           range++;
	           len_p = 1;
		}

                p[0] = c;
                memcpy(p_c,p,len_p);
            }
	}

	code=range;
	for(i=0;((i<range) && (code==range));i++){

		if(memcmp(d[i],p,len_p) == 0)
		  code=i;
	}

	fwrite(&code,2,1,fpt2);
	printf("Compression Complete!!!!!\n");
	fclose(fpt2);
	return(0);
}

int Decompress()
{
		
	unsigned char d[65535][100];
    	unsigned char d_length[65535]; 
	unsigned short int code;
	unsigned short int code2; 
	char output[80];
    	unsigned char c;
    	unsigned char p[100];
	unsigned short int range = 256;
	int i;
	unsigned char *ptr;


        strcpy(output,input);
        strcat(output,".ulzw.");
        ptr=(unsigned char *)calloc(1,1);

        for(i=0;i<3;i++){

           fread(ptr,1,1,fpt);
           strcat(output,ptr);
        }

        if((fpt2=fopen(output,"wb"))==NULL){

            printf("Can't create output file.\n");
	    exit(0);
        }
	
	for(i=0;i<range;i++){

	    d[i][0] = i;
	    d_length[i] = 1;
	}

	fread(&code,2,1,fpt);
	fwrite(&code,d_length[code],1,fpt2);
	c=code;
	
	printf("File is decompressing......\n");

	while((feof(fpt) == 0) && (range < 65535)){

	    if(fread(&code2,2,1,fpt)==0)
		break;

    	    if(code2 >= range){

    	        memcpy(p,d[code],d_length[code]);
    	        p[d_length[code]] = c;

    	        if(fwrite(p,(d_length[code]+1),1,fpt2)==0)
		   break;
    	    }

    	    else{

    	        memcpy(p,d[code2],d_length[code2]);
    	        if(fwrite(p,d_length[code2],1,fpt2)==0)
		   break;
    	    }

    	    c=p[0];

	    if(range<65535){

	       memcpy(d[range],d[code],d_length[code]);
	       d[range][d_length[code]] = c;
	       d_length[range] = (d_length[code] + 1);
	       range++;
	    }
    	    code = code2;
    	}
	printf("Decompression Complete!!!!\n");
	free(ptr);
	fclose(fpt2);
	return(0);
}

