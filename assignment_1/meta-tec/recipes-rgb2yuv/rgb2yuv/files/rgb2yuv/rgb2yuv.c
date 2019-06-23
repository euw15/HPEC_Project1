#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>



double convertNumbers[3][3] = {{0.299,0.587,0.114},{-0.14713, -0.28886, 0.436},{0.615, -0.51499, -0.10001}};

void rgb2yuv (char *input_image, char *output_image)
{
   
}


int main (int argc, char **argv)
{
   char *outputFile = NULL;
   char *inputFile = NULL;

   int c;
   while ((c = getopt (argc, argv, ":i:o:ha")) != -1)
   {
   	switch(c)
   	{
   		case 'i':
   		{
   			inputFile = optarg;
   			printf("Input file: %s\n", inputFile); 
   			break;
   		}
   		case 'o':
   		{
   			outputFile = optarg;
   			printf("Output file %s\n", outputFile);
   			break;
   		}
   		case 'h':
   		{
   			printf("Help file\n");
   			break;
   		}
   		case 'a':
   		{
   			printf("Authors:\n Luis Arias \n Guillermo Lopez \n Edward Umana\n");
   			break;
   		}


   	}
   }

    
   return 0;
}
