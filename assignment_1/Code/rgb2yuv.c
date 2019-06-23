#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define SCALE_U         0.436*2
#define SCALE_V         0.615*2
#define MAX_BYTE_VAL    255
#define OFFSET_UV       128
#define HEIGHT          480
#define WIDTH           640


struct b_pixelRGB{
   unsigned char b;
   unsigned char g;
   unsigned char r;
};

struct f_pixelRGB{
   float b;
   float g;
   float r;
};

struct b_pixelYUV{
   unsigned char y;
   unsigned char u;
   unsigned char v;
};

struct f_pixelYUV{
   float y;
   float u;
   float v;
};

double convertNumbers[3][3] = {{0.299,0.587,0.114},{-0.14713, -0.28886, 0.436},{0.615, -0.51499, -0.10001}};

float to8bits(float val)
{
   if (val < 0)
   {
      return 0;
   }
   else if (val > 255)
   {
      return 255;
   }
   return val;
}

void rgb2yuv(char *input_image, char *output_image)
{
   int i = 0;
   int c = 0;
   int r = 0;

   // Validaciones de entrada
   if(input_image == NULL || output_image == NULL) 
   {
      printf("Set output and input file\n");
      return;
   }

   FILE* inputFile;
   inputFile = fopen(input_image, "r");
   if(inputFile == NULL){
      printf("Wrong input path, could not load input image\n");
      return;
   }

   // Reservando memoria 
   struct b_pixelRGB** RowsPixelsRGB = (struct b_pixelRGB**) calloc(sizeof(struct b_pixelRGB*), HEIGHT);
   struct b_pixelYUV** RowsPixelsYUV = (struct b_pixelYUV**) calloc(sizeof(struct b_pixelYUV*), HEIGHT);

   for (i = 0; i < HEIGHT; ++i)
   {
      RowsPixelsRGB[i] = (struct b_pixelRGB*) calloc(sizeof(struct b_pixelRGB), WIDTH);
      RowsPixelsYUV[i] = (struct b_pixelYUV*) calloc(sizeof(struct b_pixelYUV), WIDTH);
      
      // Leyendo la imagen completa
      fread(RowsPixelsRGB[i], sizeof(struct b_pixelRGB), WIDTH, inputFile);
   }

   /*
   // Leyendo la imagen completa
   for(i = 0; i < h; i++)
   {
      fread(RowsPixelsRGB[i], sizeof(struct b_pixelRGB), WIDTH, inputFile);
   }
   */

   fclose(inputFile);

   // Validación de Output file
   FILE* oFile;
   oFile = fopen(output_image, "w");
   if(oFile == NULL){
      printf("Wrong output path, could not load/create output image\n");
      return;
   }

   // Conversión RGB a YUV
   struct b_pixelRGB b_pixRGB;
   struct f_pixelRGB f_pixRGB;
   struct f_pixelYUV f_pixYUV;
   for(r = 0; r < HEIGHT; r++)
   {
      for(c = 0; c < WIDTH; c++)
      {
         b_pixRGB = RowsPixelsRGB[r][c];

         // Pasar de 8 bit 0 a 255 a float entre 0 y 1
         f_pixRGB.r = ((float) b_pixRGB.r)/MAX_BYTE_VAL;
         f_pixRGB.g = ((float) b_pixRGB.g)/MAX_BYTE_VAL;
         f_pixRGB.b = ((float) b_pixRGB.b)/MAX_BYTE_VAL;

         // Calculo YUV
         f_pixYUV.y = convertNumbers[0][0]*f_pixRGB.r + convertNumbers[0][1]*f_pixRGB.g + convertNumbers[0][2]*f_pixRGB.b;
         f_pixYUV.u = convertNumbers[1][0]*f_pixRGB.r + convertNumbers[1][1]*f_pixRGB.g + convertNumbers[1][2]*f_pixRGB.b;
         f_pixYUV.v = convertNumbers[2][0]*f_pixRGB.r + convertNumbers[2][1]*f_pixRGB.g + convertNumbers[2][2]*f_pixRGB.b;
   
         // Pasar de escala YUV a 255
         f_pixYUV.y = f_pixYUV.y*MAX_BYTE_VAL;
         f_pixYUV.u = (f_pixYUV.u/(SCALE_U))*MAX_BYTE_VAL+OFFSET_UV;
         f_pixYUV.v = (f_pixYUV.v/(SCALE_V))*MAX_BYTE_VAL+OFFSET_UV;

         // Asegurarse de tener sólo 8 bits
         f_pixYUV.y = to8bits(f_pixYUV.y);         
         f_pixYUV.u = to8bits(f_pixYUV.u);
         f_pixYUV.v = to8bits(f_pixYUV.v);

         // Pasar YUV de float a 8 bits
         RowsPixelsYUV[r][c].y = (unsigned char) f_pixYUV.y;
         RowsPixelsYUV[r][c].u = (unsigned char) f_pixYUV.u;
         RowsPixelsYUV[r][c].v = (unsigned char) f_pixYUV.v;
      }
   }

   for(int r = 0; r < HEIGHT; r++)
   {
      fwrite(RowsPixelsYUV[r], sizeof(struct b_pixelYUV), WIDTH, oFile);
   }

   fclose(oFile);

   // Liberando memoria
   for (i = 0; i < HEIGHT; ++i)
   {
      free(RowsPixelsRGB[i]);
      free(RowsPixelsYUV[i]);
   }

   free(RowsPixelsRGB);
   free(RowsPixelsYUV);


}


int main (int argc, char **argv)
{
   char *outputFile = NULL;
   char *inputFile = NULL;

   clock_t start, end;
   double cpu_time_used;

   int c;
   while ((c = getopt (argc, argv, ":i:o:hat")) != -1)
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
            printf("Output file: %s\n", outputFile);
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
         case 't':
         {

            double average = 0.0;
            for(int i = 0; i < 100; i++)
            {
               start = clock();

               rgb2yuv(inputFile, outputFile);
               
               end = clock();
               cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
               average += cpu_time_used;
            }
            average/=100;

            printf("\nAverage %fs\n", cpu_time_used);
            break;
         }


      }
   }

   start = clock();

   rgb2yuv(inputFile, outputFile);
   
   end = clock();
   cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

   printf("\nTime elapsed: %fs\n", cpu_time_used);
   return 0;
}
