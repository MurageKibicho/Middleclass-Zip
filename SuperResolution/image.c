#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "dct.h"
#include "gmp.h"

double *logLookup;
double *GenerateLookupTable(int size)
{
	int length = 1 << size;
	double increase = 1/(double)length;
	double *table = calloc(length, sizeof(double));
	for(int i = 1; i < length; i++)
	{
		table[i] = log2(1 + increase*i);
	}
	return table;	
}
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

void AddBinary(int number, int *array, int startIndex){int j = startIndex + 7;for(int i = 0; i < 8; i++, j--){if(number % 2 == 0){array[j] = 0;}else{array[j] = 1;}number /= 2;}}
double FileForma_GammaApproximation(double n, double k){double result = 0;result = (lgamma(n+1) -lgamma(n-k+1) -lgamma(k+1)) / log(2);return result;}
int FindLargestNLogarithm(double searchLog, int base){int max = 1 << 30;int min = 0;int mid = 0;double currentLog = 0;double k = (double)base;while(min <= max){mid = min + (max - min) / 2;currentLog = FileForma_GammaApproximation((double)mid, k);if(currentLog < searchLog){min = mid + 1;}else{max = mid - 1;}}return max;}
double Find_Log_MPZ_Double(mpz_t x){signed long int ex;const double di = mpz_get_d_2exp(&ex, x);return ( (log(di) + log(2) * (double) ex) /log(2));}
void QuantizeArray(int length, int *array, int number){for(int i = 0; i < length; i++){array[i] /= number;}}
void PrintArray(int length, int *array){for(int i = 0; i < length; i++){printf("%3d,", array[i]);} printf("\n");}
void BinaryToInteger(int binaryLength, int *binary, mpz_t integer, int *oneCount)
{
	*oneCount = 1;
	mpz_t binomialCoefficient;
	mpz_t sumHolder;
	mpz_init(sumHolder); mpz_init(binomialCoefficient);
	for(int i = 0; i < binaryLength; i++)
	{
		*oneCount += binary[i];
		if(binary[i]==1)
		{
			mpz_bin_uiui(binomialCoefficient, i, *oneCount);
			mpz_add(sumHolder, sumHolder, binomialCoefficient);
		}
	}
	mpz_set(integer,sumHolder);
	mpz_clear(sumHolder); mpz_clear(binomialCoefficient);
}

void PrintFileBinary(int *file, int fileLength)
{
	int oneCount = 0;
	for(int i = 0; i < fileLength; i+=8)
	{
		printf("(%d)", i / 8);
		for(int j = 0; j < 8; j++)
		{	
			oneCount += file[i+j];
			printf("%d", file[i+j]);
		}
		//break;
		printf("(%d %d : %.3f)\n", fileLength, oneCount, FileForma_GammaApproximation((double) fileLength, (double) oneCount));
	}
}
void IntegerToBinary(mpz_t integer, int originalLength, int newOneCount, int *binaryHolder)
{
	double currentLogValue = 0.0f; int n = 0; int k = newOneCount;
	mpz_t binomialCoefficient;mpz_init(binomialCoefficient);
	while(k > 0)
	{
		currentLogValue = Find_Log_MPZ_Double(integer); 
		n =  FindLargestNLogarithm(currentLogValue, k);
		//assert(n > -1);
		if(n < 0)
		{
			//gmp_printf("%Zd \n",integer);
			break;
			//exit(1);
		}
		//mpz_setbit(binaryHolder, n);
		
		mpz_bin_uiui(binomialCoefficient, n, k);
		mpz_sub(integer, integer, binomialCoefficient);
		//gmp_printf("%Zd\n",integer);
		//printf("%d %d\n",n/3300, k);
		binaryHolder[k-1] = n;
		k-=1;
	}
	mpz_clear(binomialCoefficient);
}

#define bitLength 8

//FindRange function
void DequantizeArray(int oneCount, int length, int *array, int number)
{
	printf(" \n");
	for(int i = 0; i < length; i++){array[i] *= number;}
	double a = FileForma_GammaApproximation((double) array[length-1],(double)length) ;
	double b = FileForma_GammaApproximation((double) array[length-2],(double)length-1);
	double currentLog = 0;
	for(int i = 0; i < 1; i++)
	{
		for(int j = 0; j < 1 << bitLength; j++)
		{
			printf("%d %.3f\n", j, logLookup[j]);
		}
		/*for(int j = 0 ; j < number; j++)
		{
			currentLog = FileForma_GammaApproximation((double) array[length-1-i]+j,(double)length-i);
			printf("%d : %3d %d %.3f\n",j, array[length-1-i]+j, length-i, currentLog);
		}*/
	}
	printf("%.3f %.3f %d %d\n",a,  b,array[length-1],length);
}

void EncodeYComponent(float *component)
{
	mpz_t integer; mpz_init(integer);

	int binary[64*8] = {0};int oneCount = 0;
	for(int i = 0; i < 64; i++)
	{
		AddBinary((int) component[i], binary, i * 8);
	}
	BinaryToInteger(64*8, binary, integer, &oneCount);
	
	int newOneCount = 30;
	int *binaryHolder = calloc(newOneCount, sizeof(int));
	int newBinaryLength = FindLargestNLogarithm((double) 64 * 8, newOneCount);
	
	printf("(%d %d) - (%d %d)\n", 64*8, oneCount,newBinaryLength, newOneCount);
	IntegerToBinary(integer, 64*8, newOneCount, binaryHolder);
	PrintArray(newOneCount,binaryHolder);
	QuantizeArray(newOneCount,binaryHolder,3300);
	DequantizeArray(oneCount, newOneCount,binaryHolder,3300);
	PrintArray(newOneCount,binaryHolder);
	mpz_clear(integer);
	free(binaryHolder);
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
			printf("\n");Print3(componentY);printf("\n");
			EncodeYComponent(componentY);
			int level = 5;
			ForwardDCTComponent(componentCB);ForwardDCTComponent(componentCR);
			CBCRQuantization(componentCB,level);CBCRQuantization(componentCR,level);
			InverseDCTComponent(componentCB);InverseDCTComponent(componentCR);									
			for(int l = 0; l < 64; l++)
			{
				image[k]   = (unsigned char) componentY[l];
				image[k+1] = (unsigned char) componentCB[l];
				image[k+2] = (unsigned char) componentCR[l];
				k+=3;
			}
			break;
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
	logLookup = GenerateLookupTable(bitLength);
	char *fileName = "sample1.jpg";
	OpenImage(fileName);
	free(logLookup);
	return 0;
}
