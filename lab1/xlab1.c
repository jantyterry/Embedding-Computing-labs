#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>

#define blue 0x1F 
#define red 0xF8
#define green 0xE0
#define alpha 0x07


main(int argc, char *argv[])
{
Display			*Monitor;
Window			ImageWindow;
GC				ImageGC;
XWindowAttributes		Atts;
XImage			*Picture;
int				ROWS,COLS;
unsigned char		*displaydata;

/* ... */
FILE *fpt, *fpt2;
char Header[80];
unsigned char *input_image;
int BYTES,i;

	if((fpt = fopen(argv[1],"rb")) == NULL){
	   printf("Unable to open %s for reading\n",argv[1]);
	   exit(0);
	}
		
	
	fscanf(fpt,"%s %d %d %d",Header,&COLS,&ROWS,&BYTES);
	printf("image property: %s %d %d %d\n",Header,COLS,ROWS,BYTES);
	Header[79] = fgetc(fpt);

	displaydata = (unsigned char *)calloc((ROWS*COLS*2),1);

	if(Header[1] == '6'){	
	printf("image loaded is a 24 bit RGB with size %d x %d\n", COLS, ROWS);
	}

	else{
	printf("image loaded is a 8 bit greyscale with size %d x %d\n", COLS, ROWS);
	}

	if(Header[1] == '6'){
	  input_image = (unsigned char *)calloc((ROWS*COLS*3),1);
	  fread(input_image,1,(ROWS*COLS*3),fpt);
	 
	  for(i=0;i<(ROWS*COLS);i++){
	          displaydata[(i*2)+0] = input_image[(i*3)+2]>>3;
                  displaydata[(i*2)+1] = input_image[(i*3)+0];
		  displaydata[(i*2)+1] = displaydata[(i*2)+1] & red;
		  displaydata[(i*2)+0] = displaydata[(i*2)+0] & blue;
		  displaydata[(i*2)+1] = displaydata[(i*2)+1] | ((input_image[(i*3)+1]>>5) & alpha);
		  displaydata[(i*2)+0] = displaydata[(i*2)+0] | ((input_image[(i*3)+1]<<3) & green);
  		  	  
	  }
		
		fpt2 = fopen("test_image1.ppm", "wb");
		fprintf(fpt2, "P6 %d %d 255\n", COLS, ROWS);
		fwrite(displaydata,1,ROWS*COLS*3,fpt2);
		fclose(fpt2);
	}


	else{

	  input_image = (unsigned char *)calloc((ROWS*COLS),1);
	  fread(input_image,1,(ROWS*COLS),fpt);

	  for(i=0;i<(ROWS*COLS);i++){
	          displaydata[(i*2)+0] = input_image[i]>>3;
                  displaydata[(i*2)+1] = input_image[i];
		  displaydata[(i*2)+1] = displaydata[(i*2)+1] & red;
		  displaydata[(i*2)+0] = displaydata[(i*2)+0] & blue;
		  displaydata[(i*2)+1] = displaydata[(i*2)+1] | ((input_image[i]>>5) & alpha);
		  displaydata[(i*2)+0] = displaydata[(i*2)+0] | ((input_image[i]<<3) & green);
  		  	  
	  }
		fpt2 = fopen("test_image2.ppm", "wb");
		fprintf(fpt2, "P5 %d %d 255\n", COLS, ROWS);
		fwrite(displaydata,1,ROWS*COLS,fpt2);
		fclose(fpt2);

	}
	
	fclose(fpt);
	

Monitor=XOpenDisplay(NULL);
if (Monitor == NULL)
  {
  printf("Unable to open graphics display\n");
  exit(0);
  }

ImageWindow=XCreateSimpleWindow(Monitor,RootWindow(Monitor,0),
	50,10,		/* x,y on screen */
	COLS,ROWS,	/* width, height */
	2, 		/* border width */
	BlackPixel(Monitor,0),
	WhitePixel(Monitor,0));

ImageGC=XCreateGC(Monitor,ImageWindow,0,NULL);

XMapWindow(Monitor,ImageWindow);
XFlush(Monitor);
while(1)
  {
  XGetWindowAttributes(Monitor,ImageWindow,&Atts);
  if (Atts.map_state == IsViewable /* 2 */)
    break;
  }

Picture=XCreateImage(Monitor,DefaultVisual(Monitor,0),
		DefaultDepth(Monitor,0),
		ZPixmap,	/* format */
		0,		/* offset */
		displaydata,/* the data */
		COLS,ROWS,	/* size of the image data */
		16,		/* pixel quantum (8, 16 or 32) */
		0);		/* bytes per line (0 causes compute) */

XPutImage(Monitor,ImageWindow,ImageGC,Picture,
		0,0,0,0,	/* src x,y and dest x,y offsets */
		COLS,ROWS);	/* size of the image data */




XFlush(Monitor);
sleep(4);
XCloseDisplay(Monitor);
free(input_image);
free(displaydata);
exit(0);
}
