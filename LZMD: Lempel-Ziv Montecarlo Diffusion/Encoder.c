#include "LZMD.h"

int main()
{
	/*Open file as a binary string*/
	char *fileName = "build.jpg";
	int fileSize = GetFileSize(fileName)*8;
	int *fileContent  = calloc(fileSize, sizeof(int)); 
	int c = 0;int sampleIndex = 0;FILE *fp = fopen(fileName, "rb");assert(fp != NULL);while((c = fgetc(fp)) != EOF){AddBinary(c, fileContent, sampleIndex * 8);sampleIndex+= 1;}fclose(fp);
	
	/*Integer to store log2 size of space of empty spaces*/
	int spaceFound = 0;
	
	/*Split file into two parts. Left and Right*/
	int leftBitSize = 64;
	int leftBinaryLength = fileSize / 2;
	int leftArrayLength = 0;
	MpzWithIndex *leftArray  = CreateArray(leftBitSize,   leftBinaryLength, fileContent, &leftArrayLength);
	assert(leftArrayLength > -1); assert(leftArray != NULL);
	
	
	int rightBitSize = leftBitSize + 1;
	int rightBinaryLength = fileSize - leftBinaryLength;
	int rightArrayLength = 0;
	MpzWithIndex *rightArray = CreateArray(rightBitSize, rightBinaryLength, fileContent + leftBinaryLength, &rightArrayLength);
	assert(rightArrayLength > -1);assert(rightArray != NULL);
	
	/*Create Random Lookup Table with Finite Field Arithmetic*/
	int seed = 189009;
	int stepBits = 20;
	assert(stepBits < 31);
	int stepSize = 1 << stepBits;/*This is lookup table length*/
	MpzWithIndex *lookupTable = GenerateMixLookup(leftBitSize, seed, stepSize);
	
	/*Make leftArray super random*/
	int randomStep = 807;
	int randomSeed = 45607;
	MontecarloMakeRandom(randomSeed, randomStep, leftBitSize, leftArrayLength, leftArray);

	/*Search for empty spaces in leftArray*/
	spaceFound = leftBitSize;
	FindSpaceLeftArray(leftArrayLength, leftArray, stepSize, lookupTable, &spaceFound);

	/*Integer to store gains in bits*/	
	int gainCounter = 0;	
	MixLeftAndRight(&gainCounter, leftBitSize, rightBitSize, spaceFound, stepSize, rightArrayLength, rightArray);

	/*Write To File*/
	int toWriteLeftLength = leftArrayLength + gainCounter;
	MpzWithIndex *toWriteLeft = PrepareToWriteLeft(toWriteLeftLength, leftBitSize, leftArrayLength, leftArray, rightBitSize, rightArrayLength, rightArray,stepSize, lookupTable);

	DecodeToWriteLeft(stepSize, lookupTable, spaceFound, toWriteLeftLength, toWriteLeft);
	printf("%d %d %d %d %d\n",leftBitSize,spaceFound,leftBitSize - spaceFound,gainCounter, rightArrayLength);
	/*Free memory*/
	DestroyArray(toWriteLeftLength, toWriteLeft);
	DestroyArray(leftArrayLength, leftArray);
	DestroyArray(rightArrayLength, rightArray);
	DestroyArray(stepSize, lookupTable);
	free(fileContent);
	
	return 0;
}
