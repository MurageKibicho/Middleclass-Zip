#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <gmp.h>

FILE *fr;
int frBitIndex = 0;
unsigned char frByte = 0;
typedef struct mpz_index_struct MpzWithIndex;
struct mpz_index_struct
{
	mpz_t integer;
	int index;
	int stepIndex;
};

int MpzWithIndexCompareIndex(const void *a, const void *b){const MpzWithIndex *mpza = (const MpzWithIndex *)a;const MpzWithIndex *mpzb = (const MpzWithIndex *)b;return mpza->index - mpzb->index;}
int MpzWithIndexCompareMpzInteger(const void *a, const void *b){const MpzWithIndex *mpza = (const MpzWithIndex *)a;const MpzWithIndex *mpzb = (const MpzWithIndex *)b;return mpz_cmp(mpzb->integer, mpza->integer);}
int MpzWithIndexCompareMpzIntegerOpposite(const void *a, const void *b){const MpzWithIndex *mpza = (const MpzWithIndex *)a;const MpzWithIndex *mpzb = (const MpzWithIndex *)b;return mpz_cmp(mpza->integer, mpzb->integer);}
double Find_Log_MPZ_Double(mpz_t x){if(mpz_cmp_ui(x, 0) == 0){return 0;}signed long int ex;const double di = mpz_get_d_2exp(&ex, x);return ( (log(di) + log(2) * (double) ex) /log(2));}
int Find_Log_MPZ(mpz_t x){if(mpz_cmp_ui(x, 0) == 0){return 0;}signed long int ex;const double di = mpz_get_d_2exp(&ex, x);return (int) ceil( (log(di) + log(2) * (double) ex) /log(2));}
void AddBinary(int number, int *array, int startIndex){int j = startIndex + 7;for(int i = 0; i < 8; i++, j--){if(number % 2 == 0){array[j] = 0;}else{array[j] = 1;}number /= 2;}}
int GetFileSize(char *fileName){FILE *fp = fopen(fileName, "rb");assert(fp != NULL);fseek(fp, 0L, SEEK_END);int currentFileSize = ftell(fp);rewind(fp);fclose(fp);return currentFileSize;}
void PrintFileBinary(int *file, int fileLength)
{
	int oneCount = 0;
	for(int i = 0; i < fileLength; i+=8)
	{
		printf("\n(%d)", i / 8);
		for(int j = 0; j < 8; j++)
		{	
			oneCount += file[i+j];
			printf("%d", file[i+j]);
		}
	}
}

void InitializeMpzWithIndex(int arrayLength, MpzWithIndex *array)
{
	for(int i = 0; i < arrayLength; i++)
	{
		mpz_init(array[i].integer);
		array[i].index = 0;
		array[i].stepIndex = 0;
	}
}

MpzWithIndex *GenerateMixLookup(int bitLength, int seed, int stepSize)
{
	assert(bitLength > -1);
	MpzWithIndex *lookupTable = malloc(stepSize * sizeof(MpzWithIndex));InitializeMpzWithIndex(stepSize, lookupTable);
	mpz_t temporary0;mpz_init(temporary0);
	mpz_t temporary1;mpz_init(temporary1);
	mpz_t temporary2;mpz_init(temporary2);mpz_setbit(temporary2, bitLength);
	gmp_randstate_t state;gmp_randinit_mt (state);gmp_randseed_ui(state, seed);
	
	for(int i = 0; i < stepSize; i++)
	{
		//mpz_init(temporary0);
		mpz_urandomb(temporary1, state, bitLength+2);
		mpz_add(temporary0,temporary0,temporary1);
		mpz_mod(temporary0, temporary0,temporary2);
		mpz_sub(lookupTable[i].integer, temporary2, temporary0);
		lookupTable[i].index = i;
		//gmp_printf("%Zd\n", lookupTable[i].integer);
	}
	
	mpz_clear(temporary0);
	mpz_clear(temporary1);mpz_clear(temporary2);
	gmp_randclear(state);	
	qsort(lookupTable, stepSize, sizeof(MpzWithIndex), MpzWithIndexCompareMpzIntegerOpposite);
	return lookupTable;
}
MpzWithIndex *CreateArray(int bitSize, int binaryLength, int *binary, int *arrayLength)
{
	*arrayLength = binaryLength / bitSize;
	MpzWithIndex *array = malloc(*arrayLength * sizeof(MpzWithIndex));
	InitializeMpzWithIndex(*arrayLength, array);
	int currentBitIndex = 0;
	int currentIntegerIndex = 0;
	int i = 0;
	for(i = 0; i < binaryLength - bitSize; i += bitSize, currentIntegerIndex += 1)
	{
		currentBitIndex = bitSize-1;
		assert(currentIntegerIndex < *arrayLength);
		array[currentIntegerIndex].index = currentIntegerIndex;
		for(int j = 0; j < bitSize; j++, currentBitIndex -=1)
		{
			assert(i+j < binaryLength);
			if(binary[i+j] == 1)
			{
				mpz_setbit(array[currentIntegerIndex].integer, currentBitIndex);
			}
		}
	}
	//printf("%d %d\n", binaryLength, binaryLength - (currentIntegerIndex *bitSize));
	return array;
}

void DestroyArray(int arrayLength, MpzWithIndex *array)
{
	for(int i = 0; i < arrayLength; i++)
	{
		mpz_clear(array[i].integer);
	}
	free(array);
}

void PrintMpzWithIndex(int arrayLength, MpzWithIndex *array)
{
	for(int i = 0 ; i < arrayLength; i++)
	{
		gmp_printf("%4d : %8Zd\n", array[i].index,array[i].integer);
		//gmp_printf("%4d : %.3f\n", array[i].index, Find_Log_MPZ_Double(array[i].integer));
	}
}

void PrintMpzWithStepIndex(int arrayLength, MpzWithIndex *array)
{
	for(int i = 0 ; i < arrayLength; i++)
	{
		gmp_printf("%4d : %8Zd\n", array[i].index,array[i].integer);
		//gmp_printf("%4d : %.3f\n", array[i].stepIndex, Find_Log_MPZ_Double(array[i].integer));
	}
}

void MakeRandom(int randomSeed, int randomStep, int leftBitSize, int leftLength, MpzWithIndex *leftArray)
{
	mpz_t temporary0;mpz_init(temporary0);
	mpz_t temporary1;mpz_init(temporary1);
	mpz_t temporary2;mpz_init(temporary2);mpz_setbit(temporary2, leftBitSize);
	gmp_randstate_t state;gmp_randinit_mt (state);gmp_randseed_ui(state, randomSeed);
	
	for(int i = 0; i < randomStep; i++)
	{
		mpz_urandomb(temporary1, state, leftBitSize+2);
		mpz_add(temporary0,temporary0,temporary1);
	}
	for(int i = 0; i < leftLength; i++)
	{
		mpz_add(leftArray[i].integer, leftArray[i].integer, temporary0);
		mpz_mod(leftArray[i].integer, leftArray[i].integer, temporary2);
	}
	mpz_clear(temporary0);
	mpz_clear(temporary1);mpz_clear(temporary2);
	gmp_randclear(state);	
}

void MixArrays(int leftLength, MpzWithIndex *leftArray, int rightBitSize, int rightLength, MpzWithIndex *rightArray, int stepSize, MpzWithIndex *lookupTable , int *spaceFound)
{
	qsort(leftArray, leftLength, sizeof(MpzWithIndex), MpzWithIndexCompareMpzIntegerOpposite);
	qsort(rightArray, rightLength, sizeof(MpzWithIndex), MpzWithIndexCompareMpzIntegerOpposite);
	//PrintMpzWithIndex(stepSize, lookupTable);
	//PrintMpzWithIndex(5, rightArray);
	//PrintMpzWithIndex(rightLength, rightArray);
	int currentIndex = 0;
	int leftIndex = 0;
	mpz_t maximum; mpz_init(maximum);
	mpz_setbit(maximum, leftLength);
	mpz_t findSpace; mpz_init(findSpace);
	for(int i = 0; i < leftLength && currentIndex < stepSize; i++)
	{
		int comparison = mpz_cmp(lookupTable[currentIndex].integer, leftArray[i].integer);
		if(comparison < 0)
		{
			mpz_sub(findSpace,leftArray[i].integer,lookupTable[currentIndex].integer);
			int c = mpz_cmp(maximum, findSpace);
			if(c > 0){mpz_set(maximum, findSpace);}
			//gmp_printf("%d %Zd %Zd %Zd %Zd\n",c,maximum,findSpace,lookupTable[currentIndex].integer,leftArray[i].integer);
			currentIndex += 1;
		}
		//gmp_printf("%Zd %Zd\n",lookupTable[currentIndex].integer,leftArray[i].integer);
	}
	
	int maxSpace = (int) floor(Find_Log_MPZ_Double(maximum));
	int availableSize = maxSpace + (int) log2((double) stepSize);
	assert(availableSize >= rightBitSize);
	*spaceFound = maxSpace;
	printf("%d %d %d\n", maxSpace, availableSize, rightBitSize);
	mpz_set_ui(maximum,0);mpz_setbit(maximum, maxSpace);
	mpz_set_ui(findSpace,0);
	
	mpz_t difference; mpz_init(difference);
	mpz_t previousMaximum; mpz_init(previousMaximum);
	int currentRightIndex = 0;
	qsort(lookupTable, stepSize, sizeof(MpzWithIndex), MpzWithIndexCompareIndex);
	for(int i = 0; i < stepSize; i++)
	{
		if(currentRightIndex >= rightLength){break;}
		mpz_set(previousMaximum, findSpace);
		mpz_add(findSpace, findSpace, maximum);
		int comparison = mpz_cmp(findSpace, rightArray[currentRightIndex].integer);
		while(comparison > 0)
		{
			rightArray[currentRightIndex].stepIndex = i;
			mpz_sub(rightArray[currentRightIndex].integer, rightArray[currentRightIndex].integer, previousMaximum);
			gmp_printf("%d %.3f %.3f\t | \t",i, Find_Log_MPZ_Double(lookupTable[i].integer), Find_Log_MPZ_Double(rightArray[currentRightIndex].integer));
			mpz_add(rightArray[currentRightIndex].integer,rightArray[currentRightIndex].integer, lookupTable[i].integer);
			gmp_printf("%d %.3f %.3f\n\n",i, Find_Log_MPZ_Double(lookupTable[i].integer), Find_Log_MPZ_Double(rightArray[currentRightIndex].integer));	
			currentRightIndex += 1;
			if(currentRightIndex == rightLength){break;}
			comparison = mpz_cmp(findSpace, rightArray[currentRightIndex].integer);
		}
	}
	
	mpz_clear(findSpace);
	mpz_clear(maximum);
	mpz_clear(difference);
	mpz_clear(previousMaximum);
	qsort(leftArray, leftLength, sizeof(MpzWithIndex), MpzWithIndexCompareIndex);
	qsort(rightArray, rightLength, sizeof(MpzWithIndex), MpzWithIndexCompareIndex);
}


void AddBitToByte(int bit)
{
	//printf("%d %d | ",bit, *bitIndex);
	frByte <<= 1;
	frByte += (unsigned char) bit;
	frBitIndex += 1;	
	if(frBitIndex == 8)
	{
		//printf("(%u)", *byte);
		fwrite(&frByte, sizeof(unsigned char), 1, fr);
		frByte = 0;
		frBitIndex = 0;
	}
}

void WriteToFile(int bitSize, int length, MpzWithIndex *array)
{
	for(int i = 0; i < length; i++)
	{
		char *string     = mpz_get_str(NULL, 2,array[i].integer);
		int stringLength = mpz_sizeinbase(array[i].integer,2);
		int padding      = bitSize - stringLength;
		//gmp_printf("%d %d %Zd %s\n",padding,stringLength, leftArray[i].integer, string);
		for(int j = 0 ; j < padding; j++)
		{
			AddBitToByte(0);
		}
		for(int j = 0 ; j < stringLength; j++)
		{
			if(string[j] == '0')
			{
				AddBitToByte(0);
			}
			else
			{
				AddBitToByte(1);
			}
		}
		free(string);
	}
	
}

int main()
{
	char *fileName = "cat02.jpg";
	int fileSize = GetFileSize(fileName)*8;
	int *fileContent  = calloc(fileSize, sizeof(int)); 
	int c = 0;int sampleIndex = 0;FILE *fp = fopen(fileName, "rb");assert(fp != NULL);while((c = fgetc(fp)) != EOF){AddBinary(c, fileContent, sampleIndex * 8);sampleIndex+= 1;}fclose(fp);
	
	int spaceFound = 0;
	int leftBitSize = 38;
	int leftBinaryLength = fileSize / 2;
	
	int rightBitSize = leftBitSize + 1;
	int rightBinaryLength = fileSize - leftBinaryLength;
	assert(rightBinaryLength >= leftBinaryLength);
	assert(rightBitSize >= leftBitSize);

	int leftArrayLength = 0;
	MpzWithIndex *leftArray  = CreateArray(leftBitSize,   leftBinaryLength, fileContent, &leftArrayLength);assert(leftArrayLength > -1);
	
	int rightArrayLength = 0;
	MpzWithIndex *rightArray = CreateArray(rightBitSize, rightBinaryLength, fileContent + leftBinaryLength, &rightArrayLength);assert(rightArrayLength > -1);

	
	int seed = 8568;
	int stepBits = 18;
	int stepSize = 1 << stepBits;
	MpzWithIndex *lookupTable = GenerateMixLookup(leftBitSize, seed, stepSize);
	
	int randomStep = 100;
	int randomSeed = 10;
	//PrintMpzWithIndex(leftArrayLength, leftArray);			
	MakeRandom(randomSeed, randomStep, leftBitSize, leftArrayLength, leftArray);	

	
	MixArrays(leftArrayLength, leftArray, rightBitSize, rightArrayLength, rightArray, stepSize, lookupTable, &spaceFound);
	
	int leftRemainder  = leftBinaryLength -  (leftArrayLength  * leftBitSize );
	int rightRemainder = rightBinaryLength - (rightArrayLength * rightBitSize);
	
	//printf("%d %d\n", leftRemainder, rightRemainder);
	
	fr = fopen("m.kk", "wb");
	unsigned char byte = 'M';
	fwrite(&byte, sizeof(unsigned char), 1, fr);
	byte = 'K';
	fwrite(&byte, sizeof(unsigned char), 1, fr);
	byte = 0;
	fwrite(&byte, sizeof(unsigned char), 1, fr);
	byte = (unsigned char) leftBitSize;
	fwrite(&byte, sizeof(unsigned char), 1, fr);
	byte = (unsigned char) (rightBitSize - leftBitSize);
	fwrite(&byte, sizeof(unsigned char), 1, fr);
	byte = (unsigned char) leftRemainder;
	fwrite(&byte, sizeof(unsigned char), 1, fr);
	byte = (unsigned char) rightRemainder;
	fwrite(&byte, sizeof(unsigned char), 1, fr);
	byte = (unsigned char) spaceFound;
	fwrite(&byte, sizeof(unsigned char), 1, fr);
	byte = (unsigned char) stepBits;
	fwrite(&byte, sizeof(unsigned char), 1, fr);
	
	//PrintMpzWithIndex(rightArrayLength, rightArray);	
	printf("%d\n",rightArrayLength);
	WriteToFile(leftBitSize, leftArrayLength,  leftArray);
	WriteToFile(leftBitSize, rightArrayLength, rightArray);
	//printf("%d\n",frBitIndex);
	fclose(fr);
	DestroyArray(leftArrayLength, leftArray);
	DestroyArray(rightArrayLength, rightArray);
	DestroyArray(stepSize, lookupTable);
	free(fileContent);
	return 0;
}

