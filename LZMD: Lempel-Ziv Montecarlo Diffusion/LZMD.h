#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <gmp.h>


typedef struct mpz_index_struct MpzWithIndex;
struct mpz_index_struct
{
	mpz_t integer;
	int index;
	mpz_t stepIndex;
};

/*Qsort compare functions*/
int MpzWithIndexCompareIndex(const void *a, const void *b){const MpzWithIndex *mpza = (const MpzWithIndex *)a;const MpzWithIndex *mpzb = (const MpzWithIndex *)b;return mpza->index - mpzb->index;}
int MpzWithIndexCompareMpzInteger(const void *a, const void *b){const MpzWithIndex *mpza = (const MpzWithIndex *)a;const MpzWithIndex *mpzb = (const MpzWithIndex *)b;return mpz_cmp(mpzb->integer, mpza->integer);}
int MpzWithIndexCompareMpzIntegerOpposite(const void *a, const void *b){const MpzWithIndex *mpza = (const MpzWithIndex *)a;const MpzWithIndex *mpzb = (const MpzWithIndex *)b;return mpz_cmp(mpza->integer, mpzb->integer);}
int MpzWithIndexCompareStepIndex(const void *a, const void *b){const MpzWithIndex *mpza = (const MpzWithIndex *)a;const MpzWithIndex *mpzb = (const MpzWithIndex *)b;return mpz_cmp(mpzb->stepIndex, mpza->stepIndex);}
/*Find Logarithm of mpz_t integer. Returns 0 for log0*/
double Find_Log_MPZ_Double(mpz_t x){if(mpz_cmp_ui(x, 0) == 0){return 0;}signed long int ex;const double di = mpz_get_d_2exp(&ex, x);return ( (log(di) + log(2) * (double) ex) /log(2));}

/*FileIO functions*/
int GetFileSize(char *fileName){FILE *fp = fopen(fileName, "rb");assert(fp != NULL);fseek(fp, 0L, SEEK_END);int currentFileSize = ftell(fp);rewind(fp);fclose(fp);return currentFileSize;}
void AddBinary(int number, int *array, int startIndex){assert(number > -1); assert(number < 256);assert(array != NULL);int j = startIndex + 7;for(int i = 0; i < 8; i++, j--){if(number % 2 == 0){array[j] = 0;}else{array[j] = 1;}number /= 2;}}

/*Initialize MpzWithIndex array*/
void InitializeMpzWithIndex(int arrayLength, MpzWithIndex *array)
{
	for(int i = 0; i < arrayLength; i++)
	{
		mpz_init(array[i].integer);
		array[i].index = 0;
		mpz_init(array[i].stepIndex);
		mpz_set_si(array[i].stepIndex, -1);
	}
}

/*Destroy MpzWithIndex array*/
void DestroyArray(int arrayLength, MpzWithIndex *array)
{
	for(int i = 0; i < arrayLength; i++)
	{
		mpz_clear(array[i].integer);
		mpz_clear(array[i].stepIndex);
	}
	free(array);
}

/*Print MpzWithIndex array*/
void PrintMpzWithIndex(int arrayLength, MpzWithIndex *array)
{
	for(int i = 0 ; i < arrayLength; i++)
	{
		//gmp_printf("%4d : %8Zd\n", array[i].index,array[i].integer);
		gmp_printf("%4d : %.3f\n", array[i].index, Find_Log_MPZ_Double(array[i].integer));
	}
}

/*Convert Binary array to MpzWithIndex array*/
MpzWithIndex *CreateArray(int bitSize, int binaryLength, int *binary, int *arrayLength)
{
	assert(bitSize > 0);assert(binaryLength > 0); assert(binary != NULL);
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


/*Montecarlo simulation to make MpzWithIndex array more random */
void MontecarloMakeRandom(int randomSeed, int randomStep, int leftBitSize, int leftLength, MpzWithIndex *leftArray)
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
/*Generate Lookup Table with Finite Field Arithmetic*/
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
	return lookupTable;
}

void FindSpaceLeftArray(int leftArrayLength, MpzWithIndex *leftArray, int stepSize, MpzWithIndex *lookupTable , int *spaceFound)
{
	//Sort lookupTable by integer in descending order
	qsort(lookupTable, stepSize, sizeof(MpzWithIndex), MpzWithIndexCompareMpzIntegerOpposite);
	//Sort leftArray by integer in descending order
	qsort(leftArray, leftArrayLength, sizeof(MpzWithIndex), MpzWithIndexCompareMpzIntegerOpposite);
	
	int previousLookupIndex = 0;
	int currentLookupIndex = 0;
	mpz_t maximum; mpz_init(maximum);
	mpz_t findSpace; mpz_init(findSpace);
	mpz_setbit(maximum, *spaceFound);
	*spaceFound = 0;
	for(int i = 0; i < leftArrayLength && currentLookupIndex < stepSize; i++)
	{
		int firstComparison = mpz_cmp(lookupTable[currentLookupIndex].integer, leftArray[i].integer);
		while(firstComparison < 0)
		{
			previousLookupIndex = currentLookupIndex;
			currentLookupIndex += 1;
			if(currentLookupIndex >= stepSize){i = leftArrayLength; break;}
			firstComparison = mpz_cmp(lookupTable[currentLookupIndex].integer, leftArray[i].integer);
		}
		mpz_sub(findSpace,leftArray[i].integer,lookupTable[previousLookupIndex].integer);
		//gmp_printf("|%Zd %Zd %Zd %Zd|\n",maximum, findSpace,leftArray[i].integer,lookupTable[previousLookupIndex].integer);
		int secondComparison = mpz_cmp(maximum, findSpace);
		if(secondComparison > 0 && mpz_cmp_ui(findSpace, 0) >= 0){
		//gmp_printf("\t|%Zd %Zd %Zd|\n",maximum, leftArray[i].integer,lookupTable[previousLookupIndex].integer);
		mpz_set(maximum, findSpace);
		}
	}

	int maxSpace = (int) floor(Find_Log_MPZ_Double(maximum));
	*spaceFound = maxSpace;
	mpz_clear(findSpace);
	mpz_clear(maximum);

	//Sort lookupTable by index
	qsort(lookupTable, stepSize, sizeof(MpzWithIndex), MpzWithIndexCompareIndex);
	//Sort leftArray by index in 
	qsort(leftArray, leftArrayLength, sizeof(MpzWithIndex), MpzWithIndexCompareIndex);
}

//Use later
void PunctureLookupTable(int spaceFound, int stepSize, MpzWithIndex *lookupTable)
{
	//Sort lookupTable by integer in descending order
	qsort(lookupTable, stepSize, sizeof(MpzWithIndex), MpzWithIndexCompareMpzIntegerOpposite);
	mpz_t difference; mpz_init(difference);
	for(int i = 0; i < stepSize - 1; i++)
	{
		mpz_sub(difference,lookupTable[i+1].integer,lookupTable[i].integer);
		//gmp_printf("%.3f %.3f %.3f\n", Find_Log_MPZ_Double(difference) ,Find_Log_MPZ_Double(lookupTable[i].integer),Find_Log_MPZ_Double(lookupTable[i+1].integer));
	}
	mpz_clear(difference);
	//Sort lookupTable by index
	qsort(lookupTable, stepSize, sizeof(MpzWithIndex), MpzWithIndexCompareIndex);
}

//Place Large particles from right inside small left particles
void MixLeftAndRight(int *gainCounter, int leftBitSize,int rightBitSize, int spaceFound, int stepSize, int rightArrayLength, MpzWithIndex *rightArray)
{
	mpz_t modValue; mpz_init(modValue);
	mpz_t modulo; mpz_init(modulo);
	assert(rightBitSize - spaceFound > 0);
	assert(rightBitSize - spaceFound < 64);

	assert(rightBitSize - spaceFound < stepSize);
	mpz_setbit(modulo, rightBitSize - spaceFound);
	for(int i = 0; i < rightArrayLength; i++)
	{
		mpz_mod(modValue,rightArray[i].integer,modulo);
		if(mpz_cmp_ui(modValue, stepSize) <= 0)
		{
			mpz_set(rightArray[i].stepIndex, modValue);
			mpz_fdiv_q(rightArray[i].integer, rightArray[i].integer, modulo);
			//gmp_printf("%d %Zd %Zd\n",*gainCounter,rightArray[i].integer,  modValue);
			*gainCounter += 1;
		}
	}
	mpz_clear(modValue);mpz_clear(modulo);
}

MpzWithIndex *PrepareToWriteLeft(int toWriteLeftLength,int leftBitSize, int leftArrayLength, MpzWithIndex *leftArray, int rightBitSize, int rightArrayLength, MpzWithIndex *rightArray,int stepSize, MpzWithIndex *lookupTable)
{
	MpzWithIndex *toWriteLeft = malloc(toWriteLeftLength * sizeof(MpzWithIndex));InitializeMpzWithIndex(toWriteLeftLength, toWriteLeft);
	qsort(rightArray, rightArrayLength, sizeof(MpzWithIndex), MpzWithIndexCompareIndex);
	mpz_t temporary2;mpz_init(temporary2);mpz_setbit(temporary2, leftBitSize);
	for(int i = 0; i < rightArrayLength; i++)
	{
		if(mpz_cmp_ui(rightArray[i].stepIndex, 0) >= 0)
		{
			unsigned long index = (unsigned long)mpz_get_ui(rightArray[i].stepIndex);
			mpz_add(rightArray[i].integer,rightArray[i].integer, lookupTable[index].integer);
			mpz_mod(rightArray[i].integer, rightArray[i].integer, temporary2);
			mpz_set(toWriteLeft[i].integer, rightArray[i].integer);
			toWriteLeft[i].index = -1;
			rightArray[i].index = -1;
			//gmp_printf("%d %3u %8d : %Zd\n",rightArray[i].index, index, rightArray[i].index,rightArray[i].integer);
		}
	}	
	
	int currentLeftIndex = 0;
	for(int i = 0 ; i < toWriteLeftLength; i++)
	{
		if(toWriteLeft[i].index > -1)
		{
			mpz_set(toWriteLeft[i].integer, leftArray[currentLeftIndex].integer);
			gmp_printf("%d %Zd\n", toWriteLeft[i].index, toWriteLeft[i].integer);	
			currentLeftIndex += 1;
		}
	}
	mpz_clear(temporary2);
	return toWriteLeft;
}

void DecodeToWriteLeft(int stepSize, MpzWithIndex *lookupTable, int spaceFound, int toWriteLeftLength, MpzWithIndex *toWriteLeft)
{
	//Sort lookupTable by integer in descending order
	qsort(lookupTable, stepSize, sizeof(MpzWithIndex), MpzWithIndexCompareMpzIntegerOpposite);
	//Sort toWriteLeft by integer in descending order
	qsort(toWriteLeft, toWriteLeftLength, sizeof(MpzWithIndex), MpzWithIndexCompareMpzIntegerOpposite);
	int cout = 0;
	int previousLookupIndex = 0;
	int currentLookupIndex = 0;
	mpz_t maximum; mpz_init(maximum);
	mpz_t findSpace; mpz_init(findSpace);
	mpz_setbit(maximum, spaceFound);
	for(int i = 0; i < toWriteLeftLength && currentLookupIndex < stepSize; i++)
	{
		int firstComparison = mpz_cmp(lookupTable[currentLookupIndex].integer, toWriteLeft[i].integer);
		while(firstComparison < 0)
		{
			previousLookupIndex = currentLookupIndex;
			currentLookupIndex += 1;
			if(currentLookupIndex >= stepSize){i = toWriteLeftLength; break;}
			firstComparison = mpz_cmp(lookupTable[currentLookupIndex].integer, toWriteLeft[i].integer);
		}
		mpz_sub(findSpace,toWriteLeft[i].integer,lookupTable[previousLookupIndex].integer);
		//gmp_printf("|%Zd %Zd %Zd %Zd|\n",maximum, findSpace,leftArray[i].integer,lookupTable[previousLookupIndex].integer);
		int secondComparison = mpz_cmp(maximum, findSpace);
		if(secondComparison > 0 && mpz_cmp_ui(findSpace, 0) >= 0)
		{
			gmp_printf("%d\n", cout);
			cout += 1;
		}
	}


	mpz_clear(findSpace);
	mpz_clear(maximum);

	//Sort lookupTable by index
	qsort(lookupTable, stepSize, sizeof(MpzWithIndex), MpzWithIndexCompareIndex);


}

