#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <arm_neon.h>

#define SCALE_U         0.436*2
#define SCALE_V         0.615*2
#define MAX_BYTE_VAL    255
#define OFFSET_UV       128
#define HEIGHT          480
#define WIDTH           640
#define Y               0
#define U               1
#define V               2

struct b_pixelRGB{
   uint8_t b;
   uint8_t g;
   uint8_t r;
};

struct f_pixelRGB{
   float32_t r;
   float32_t g;
   float32_t b;
   float32_t z;
};

struct b_pixelYUV{
   uint8_t y;
   uint8_t u;
   uint8_t v;
};

struct f_pixelYUV{
   float32_t y;
   float32_t u;
   float32_t v;
   float32_t z;
};

float32_t convertNumbers[3][4] = {{0.299,0.587,0.114,0},{-0.14713, -0.28886, 0.436,0},{0.615, -0.51499, -0.10001,0}};
//float32x2_t coefs[3][2] = {{0.299,0.587,0.114,0},{-0.14713, -0.28886, 0.436,0},{0.615, -0.51499, -0.10001,0}};
float32x2_t coefs[3][2] = {{{0.299,0.587},{0.114,0}},{{-0.14713, -0.28886}, {0.436,0}},{{0.615, -0.51499}, {-0.10001,0}}};

float32_t to8bits(float32_t val)
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
   uint32x2_t tmp;

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
   
   //struct b_pixelRGB b_pixRGB;
   for (r = 0; r < HEIGHT; ++r)
   {
      RowsPixelsRGB[r] = (struct b_pixelRGB*) calloc(sizeof(struct b_pixelRGB), WIDTH);
      RowsPixelsYUV[r] = (struct b_pixelYUV*) calloc(sizeof(struct b_pixelYUV), WIDTH);
     
      fread(RowsPixelsRGB[r], sizeof(struct b_pixelRGB), WIDTH, inputFile);
/*
      for(c = 0; c < WIDTH; c++)
      {
         // Leyendo la imagen completa
         fread(&b_pixRGB, sizeof(struct b_pixelRGB), 1, inputFile);

         // Pasar de 8 bit 0 a 255 a float entre 0 y 1
         RowsPixelsRGB[r][c].r = ((float32_t) b_pixRGB.r)/MAX_BYTE_VAL;
         RowsPixelsRGB[r][c].g = ((float32_t) b_pixRGB.g)/MAX_BYTE_VAL;
         RowsPixelsRGB[r][c].b = ((float32_t) b_pixRGB.b)/MAX_BYTE_VAL;
         RowsPixelsRGB[r][c].z = 0;
      }*/
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
   float32x2_t* tmpYUV = (float32x2_t*) &f_pixYUV;

   float32x2_t* pixel;

   float32x2_t* tmpYx2 = (float32x2_t*) calloc(sizeof(float32x2_t), 2);
   float32x2_t* tmpUx2 = (float32x2_t*) calloc(sizeof(float32x2_t), 2);
   float32x2_t* tmpVx2 = (float32x2_t*) calloc(sizeof(float32x2_t), 2);

   float32x2_t tmpY;
   float32x2_t tmpU;
   float32x2_t tmpV;
   
   float32x2_t tmpZ = {0,0};

   for(r = 0; r < HEIGHT; r++)
   {
      //rowPixels = (float32x4_t*) RowsPixelsRGB[r];
      for(c = 0; c < WIDTH; c++)
      {
         b_pixRGB = RowsPixelsRGB[r][c];

         // Pasar de 8 bit 0 a 255 a float entre 0 y 1
         f_pixRGB.r = ((float32_t) b_pixRGB.r)/MAX_BYTE_VAL;
         f_pixRGB.g = ((float32_t) b_pixRGB.g)/MAX_BYTE_VAL;
         f_pixRGB.b = ((float32_t) b_pixRGB.b)/MAX_BYTE_VAL;
         f_pixRGB.z = 0.0;  

         // Calculo YUV
         
         pixel = (float32x2_t*) (&f_pixRGB); 

         tmpYx2[0] = vmul_f32(pixel[0], coefs[Y][0]);
         tmpUx2[0] = vmul_f32(pixel[0], coefs[U][0]);
         tmpVx2[0] = vmul_f32(pixel[0], coefs[V][0]);

         tmpYx2[1] = vmul_f32(pixel[1], coefs[Y][1]);
         tmpUx2[1] = vmul_f32(pixel[1], coefs[U][1]);
         tmpVx2[1] = vmul_f32(pixel[1], coefs[V][1]);

         tmpY = vpadd_f32(tmpYx2[0], tmpYx2[1]);
         tmpU = vpadd_f32(tmpUx2[0], tmpUx2[1]);
         tmpV = vpadd_f32(tmpVx2[0], tmpVx2[1]);

         tmpYUV[0] = vpadd_f32(tmpY, tmpU);
         tmpYUV[1] = vpadd_f32(tmpV, tmpZ);
         
         // Pasar de escala YUV a 255
         f_pixYUV.y = f_pixYUV.y*MAX_BYTE_VAL;
         f_pixYUV.u = (f_pixYUV.u/(SCALE_U))*MAX_BYTE_VAL+OFFSET_UV;
         f_pixYUV.v = (f_pixYUV.v/(SCALE_V))*MAX_BYTE_VAL+OFFSET_UV;

         // Asegurarse de tener sólo 8 bits
         f_pixYUV.y = to8bits(f_pixYUV.y);         
         f_pixYUV.u = to8bits(f_pixYUV.u);
         f_pixYUV.v = to8bits(f_pixYUV.v);

         // Pasar YUV de float a 8 bits
         RowsPixelsYUV[r][c].y = (uint8_t) f_pixYUV.y;
         RowsPixelsYUV[r][c].u = (uint8_t) f_pixYUV.u;
         RowsPixelsYUV[r][c].v = (uint8_t) f_pixYUV.v;
      }
   }

   for(r = 0; r < HEIGHT; r++)
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

   int i;
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
            printf("***************\n*    Usage    * \n***************\n"
                  "\n./rgb2yuv [ -i RGBfile ] [ -o YUVfile ] [ -h ] [ -a ] \n"
                  "-i RGBfile ​ \t specifies the RGB file to be converted.\n"
                  "-o YUVfile  \t specifies the output file name.\n"
                  "-a \t\t ​displays the information of the author of the program.\n"
                  "-h​ \t\t displays the usage message to let the user know how to execute the application.\n \n");
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
            for(i = 0; i < 100; i++)
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
