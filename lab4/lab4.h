#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define maxcode_len 50
#define buflen 512 

struct tree 
{
   short limb; 
   short limb1; 
};

struct b_treedata 
{
   unsigned long frequency;
   short indx; 
};

struct dictionary
{
   unsigned  char cutting; 
   char code [maxcode_len];
};


int Usage();
int compress (FILE *, FILE *,unsigned char *);
int decompress (FILE *, FILE *); 
short calculate_freq (FILE *, unsigned long *, unsigned short ext[3]); 
short read_freq ( FILE *); 
short tree_c (short); 
void dictionary_c (unsigned char*, short, short);


struct b_treedata *tree_d;
struct tree *tree;
struct dictionary *diction;

