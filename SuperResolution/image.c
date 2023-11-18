#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "dct.h"


void RGBtoYCBCR(unsigned char *r, unsigned char *g, unsigned char *b)
{
	unsigned char y  =0;unsigned char cb =0;unsigned char cr =0;	
	y = 0.299* (*r) + 0.587 * (*g)    + 0.114* (*b);
	cb= 128 - 0.168736* (*r) - 0.331364 * (*g)    + 0.5* (*b);
	cr= 128 + 0.5* (*r)  - 0.418688 * (*g)    - 0.081312* (*b);
	*r = y;*g = cb;*b = cr;
}

void ConvertPixelsToYCBCR(unsigned char *image, int arrayLength)
{
	unsigned char r =0;unsigned char g =0;unsigned char b =0;
	for(int i = 0; i < arrayLength-3; i+=3)
	{
		r = image[i];g = image[i+1];b = image[i+2];RGBtoYCBCR(&r, &g, &b);image[i]   = r;image[i+1] = g;image[i+2] = b;
	}
}

void YCBCRToRGB(unsigned char *y, unsigned char *cb, unsigned char *cr)
{
	unsigned char r  =0;unsigned char g =0;unsigned char b =0;
	
	float rFloat = 0;float gFloat = 0;float bFloat = 0;	
	rFloat =  (float)(*y) + 1.402 * ((float)*cr - 128);	
	if(rFloat < 0){rFloat = 0;}if(rFloat > 255){rFloat = 255;}
	r = (unsigned char)rFloat;
	
	gFloat = (float)(*y) - 0.34414 * ((float)*cb - 128) - 0.71414 * ((float)*cr - 128);
	if(gFloat < 0){gFloat = 0;}if(gFloat > 255){gFloat = 255;}
	g = (unsigned char)gFloat;
	
	//Handle color conversion problems b
	bFloat = (float)(*y) + 1.772 * ((float)*cb - 128);
	if(bFloat < 0){bFloat = 0;}if(bFloat > 255){bFloat = 255;}
	b = (unsigned char)bFloat;
	*y = r;*cb =g;*cr =b;
}

void ConvertPixelsToRGB(unsigned char *image, int arrayLength)
{
	unsigned char y =0;unsigned char cb =0;unsigned char cr =0;
	for(int i = 0; i < arrayLength-3; i+=3)
	{
		y = image[i];cb = image[i+1];cr = image[i+2];
		YCBCRToRGB(&y, &cb, &cr);
		image[i] = y;image[i+1] = cb;image[i+2] = cr;
	}
}



void EncodeImage(unsigned char *image, int imageLength)
{
	float componentY [64] = {0.0f};
	float componentCB[64] = {0.0f};
	float componentCR[64] = {0.0f};
	int j = 0; int count = 0; int k = 0;
	for(int i = 0; i < imageLength-3; i+=3)
	{
		componentY[j] = (float) image[i]; componentCB[j] = (float) image[i+1]; componentCR[j] = (float) image[i+2];
		j+=1;
		if(j == 64)
		{
			ForwardDCTComponent(componentCB);ForwardDCTComponent(componentCR);
			int level = 62;
			
			Print3(componentY);printf("\n");
			CBCRQuantization(componentY,level);
			//CBCRQuantization(componentCB,level);CBCRQuantization(componentCR,level);
			
			Print3(componentY);printf("End\n\n");
			//Print3(componentCB);printf("\n");
			//Print3(componentCR);printf("\n\n\n");
			
			InverseDCTComponent(componentCB);InverseDCTComponent(componentCR);			
						
			for(int l = 0; l < 64; l++)
			{
				image[k]   = (unsigned char) componentY[l];
				image[k+1] = (unsigned char) componentCB[l];
				image[k+2] = (unsigned char) componentCR[l];
				k+=3;
			}
			//break;
			j = 0; count += 1;
		}
	}
}

void OpenImage(char *fileName)
{

	int imageHeight = 0;int imageWidth = 0;int colorChannels = 0;int imageLength = 0; 
	unsigned char *image = stbi_load(fileName,&imageWidth,&imageHeight,&colorChannels,0);
	imageLength = imageHeight *imageWidth * colorChannels;

	ConvertPixelsToYCBCR(image,imageLength);
	EncodeImage(image,imageLength);
	ConvertPixelsToRGB(image,imageLength);
	
	stbi_write_jpg("u5.jpg",imageWidth,imageHeight,colorChannels,image,80);
	
	stbi_image_free(image);
}

int main()
{
	char *fileName = "sample2.jpg";
	OpenImage(fileName);
	return 0;
}
