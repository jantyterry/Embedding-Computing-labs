#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lab4.h"

char input[80];
int main (int argc, char *argv [])
{
   FILE *fpt, *fpt2;

   unsigned char ext[3];
	char t_input[80];
	int i, j;

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
    

   if ((tree_d=(struct b_treedata *)malloc(512*sizeof(struct b_treedata))) == NULL)
   {
      printf( "error in malloc\n");
      return -1;
   }

   if ((tree = ( struct tree *) malloc (512 * sizeof ( struct tree))) == NULL)
   {
      free (tree_d);
      printf( "error in malloc\n");
      return -1;
   }

   
   if(strcmp(argv[1],"-c")==0)
      compress(fpt, fpt2, ext);

   if (strcmp(argv[1],"-u")==0)
      decompress(fpt, fpt2);
   

   free (tree_d);
   free (tree);
   fclose (fpt);
   
    return 0;
}

int Usage(){
	printf("Usage:\nSpecify the file to be compressed or decompressed...\nhuff -c input  #compress file input\nhuff -u input  #decompress file input\nCompressed file will be found in the parent folder with the same name as the file but with a .huff extension\nDecompressed file will be found in the parent folder with the similar name as the file\n");

return(0);
}

int compress (FILE *fpt, FILE *fpt2, unsigned char ext[3])
{
    int i, r, oct_r, bitting_r, bcount, bitting_w; 
    unsigned  long data_size; 
    short freq_indx; 
    short t_root; 
    unsigned  short ascii = 0; 
    unsigned  char code[maxcode_len];
    unsigned  char buffer[buflen];
    char output[80];

        strcpy(output,input);
        strcat(output,".huff");
        if((fpt2=fopen(output,"wb"))==NULL){

            printf("Can't create output file.\n");
	    exit(0);
        }
        fputs((char *)ext,fpt2);



   printf("File is Compressing ... \n");

   freq_indx = calculate_freq (fpt, &data_size, &ascii);
   
   fwrite (( char *) & data_size, 4, 1, fpt2);
   fwrite (&ascii, 2, 1, fpt2);

      i = freq_indx;
      while (i !=-1)
      {
         ascii = i;
         fwrite (&ascii, 1, 1, fpt2);
         fwrite (( char *) &tree_d[i].frequency, 4, 1, fpt2);
         i =tree_d[i].indx;
      }
   
   
   t_root = tree_c(freq_indx);

   
   if((diction =(struct dictionary*)malloc(256*sizeof(struct dictionary)))==NULL)
   {
      printf ("error in malloc\n");
      return -1;
   }

   for(i = 0;i<256; i++)
   diction[i].cutting = 0;

   dictionary_c(code, t_root, 0);

   fseek(fpt, 0, SEEK_SET);
   code[0] = 0;
   bitting_w = 0x80;

   while ((r = fread (buffer, 1, buflen, fpt))> 0)
   {
 
      for (i = 0; i<r; i++)
      {
         
         oct_r = 0;
         bitting_r = 0x80;
         
         for (bcount = 0; bcount < diction[buffer[i]].cutting;bcount++)
         {
            if (diction[buffer[i]].code[oct_r] & bitting_r)
               code[0] |= bitting_w;
         
            bitting_r >>= 1;
            if (bitting_r == 0)
            {
               oct_r++;
               bitting_r = 0x80;
            }
            bitting_w >>= 1;
            if (bitting_w == 0)
            {
               fputc (code[0], fpt2);
               code [0] = 0;
               bitting_w = 0x80;
            }
         }
      }
   }
   if (bitting_w != 0x80)
    fputc (code[0], fpt2);
    free(diction);
    printf ("Compression Complete!!!!!\n");
    return(0);
}


int decompress (FILE *fpt, FILE *fpt2)
{
    int i, j, r;
    unsigned  int data_size;
    short freq_indx; 
    short t_root; 
    unsigned  char ascii, bitting_r; 
    unsigned  char buffer[buflen];
    unsigned char *ptr;
    char output[80];


        strcpy(output,input);
        strcat(output,".uhuff.");
        ptr=(unsigned char *)calloc(1,1);

        for(i=0;i<3;i++){

           fread(ptr,1,1,fpt);
           strcat(output,ptr);
        }
 	
	if((fpt2=fopen(output,"wb"))==NULL){

            printf("Can't create output file.\n");
	    exit(0);
        }

 
   
   fread (( char *) &data_size, 4, 1, fpt);
   printf ( "File is Decompression ... \n");

   freq_indx = read_freq (fpt);

   if (freq_indx == -1)
   {
      printf ( "Error reading the table of frequencies \n");
      return -1;
   }

   t_root = tree_c(freq_indx);

   j = t_root;
   while ((r = fread (buffer, 1, buflen, fpt))> 0)
   {
      
      for (i = 0; i<r && data_size> 0; i++)
      {
          
         for (bitting_r = 0x80; bitting_r != 0 && data_size>0; bitting_r >>= 1)
         {
            if (buffer[i]&bitting_r)
               j = tree[j].limb1;
            else
               j = tree[j].limb;
            if ((tree[j].limb == -1) || (tree[j].limb1 == -1))
            {              
               fputc ((char)j, fpt2);
               data_size--;
               j = t_root;
            }
         }
      }
   }
    printf ("Decompression Complete!!!!! \n");
    return(0);
}

short calculate_freq (FILE *fpt, unsigned long *oct_n, unsigned short *ascii)
{
    int i, r;
    unsigned char buffer[buflen];
    short freq_indx = 0, precedent = -1;

   int contue = 1;
   short c1, c2;

   *oct_n = 0;
   *ascii = 0;

   memset (tree_d, 0, 512*sizeof( struct b_treedata));
 
   while ((r = fread (buffer, 1, buflen, fpt))> 0)
   {
      *oct_n += r;
      for (i = 0; i<r; i++)
         tree_d[buffer[i]].frequency++;
   }

   for (i = 0; i <256; i ++)
       if (tree_d[i].frequency> 0)
      {
         (*ascii) ++;
         if (precedent == -1)
            freq_indx = i;
         else
            tree_d[precedent].indx = i;
         precedent = i;
      }
   if (precedent == -1)
      freq_indx = -1;
     else
      tree_d[precedent].indx = -1;

   while (contue)
   {
      c1 = freq_indx;
      contue = 0;
      precedent = -1;
      while (c1 != -1)
      {
         if ((c2 = tree_d[c1].indx) !=- 1)
         {
            if (tree_d[c1].frequency> tree_d[c2].frequency)
            {
               contue = 1;
               if (precedent == -1)
                  freq_indx = c2;
               else
                  tree_d [precedent].indx = c2;
               tree_d [c1].indx = tree_d[c2].indx;
               tree_d [c2].indx = c1;
            }
            precedent = c1;
            c1 = c2;
         }
         else
            c1 = c2;
      }
   }
   return freq_indx;
}



short read_freq (FILE *fpt)
{
    unsigned  short ascii;
    unsigned  char i;
    short freq_indx = -1, precedent = -1;

   fread (& ascii, 2, 1, fpt);

   while (ascii> 0)
   {
      fread (&i, 1, 1, fpt);
      fread (( char *) &tree_d[i].frequency, 4, 1, fpt); 
      if (freq_indx == -1)
         freq_indx = i;
      else
         tree_d[precedent].indx = i;
      precedent = i;
      ascii--;
   }
   if (precedent == -1)
       return -1;
   tree_d [precedent].indx =-1;
   return freq_indx;
}


short tree_c (short freq_indx)
{
    short i, j, j_store;
    unsigned  long total_freq;
    short node_n = 256;
    char struct_ins = 0;

   for (j = 0; j <256; j++)
      tree[j].limb=tree[j].limb1=-1;

   i = freq_indx;
   while (i !=-1)
   {
      if (tree_d[i].indx ==-1)
      {
         break ;
      }

      total_freq = tree_d[i].frequency +tree_d[tree_d[i].indx].frequency;
  
      tree_d[node_n].frequency = total_freq;
      tree[node_n].limb = tree_d[i].indx;
      tree[node_n].limb1 = i;
      
      j_store =-1;
      struct_ins = 0;
      j = i;
      while (j !=-1 && struct_ins == 0)
      {
         if (tree_d[j].frequency >= total_freq)
         {
            if (j_store !=-1)
               tree_d[j_store].indx = node_n;
            tree_d[node_n].indx = j;
 
            struct_ins = 1;
         }
         j_store = j;
         j = tree_d[j].indx;
      }
      
      if (struct_ins == 0)
      {
         tree_d[j_store].indx = node_n;
         tree_d[node_n].indx =-1;
      }
      node_n++;
      i = tree_d[i].indx;
      i = tree_d[i].indx;
   }
   
   return node_n-1;
}


void dictionary_c (unsigned char *code, short index, short pos)
{ 
   if ((tree[index].limb == -1) && (tree[index].limb1 ==-1))
   {
      memcpy (diction[index].code, code, maxcode_len);
      diction[index].cutting = ( unsigned  char ) pos ;
   }
   
   else
   {
      code [pos / 8] &=~ (0x80 >> (pos%8));
      dictionary_c(code, tree[index].limb, ( short ) (pos + 1));
      code [pos / 8] |=0x80 >> (pos %8);
      dictionary_c (code, tree[index].limb1, ( short ) (pos + 1));
   }
}

