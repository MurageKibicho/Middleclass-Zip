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
	int stepIndex;
};

/*Qsort compare functions*/
int MpzWithIndexCompareIndex(const void *a, const void *b){const MpzWithIndex *mpza = (const MpzWithIndex *)a;const MpzWithIndex *mpzb = (const MpzWithIndex *)b;return mpza->index - mpzb->index;}
int MpzWithIndexCompareMpzInteger(const void *a, const void *b){const MpzWithIndex *mpza = (const MpzWithIndex *)a;const MpzWithIndex *mpzb = (const MpzWithIndex *)b;return mpz_cmp(mpzb->integer, mpza->integer);}
int MpzWithIndexCompareMpzIntegerOpposite(const void *a, const void *b){const MpzWithIndex *mpza = (const MpzWithIndex *)a;const MpzWithIndex *mpzb = (const MpzWithIndex *)b;return mpz_cmp(mpza->integer, mpzb->integer);}

/*Find Logarithm of mpz_t integer. Returns 0 for log0*/
double Find_Log_MPZ_Double(mpz_t x){if(mpz_cmp_ui(x, 0) == 0){return 0;}signed long int ex;const double di = mpz_get_d_2exp(&ex, x);return ( (log(di) + log(2) * (double) ex) /log(2));}

/*FileIO functions*/
int GetFileSize(char *fileName){FILE *fp = fopen(fileName, "rb");assert(fp != NULL);fseek(fp, 0L, SEEK_END);int currentFileSize = ftell(fp);rewind(fp);fclose(fp);return currentFileSize;}

/*Convert Binary array to MpzWithIndex array*/

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

/*Initialize MpzWithIndex array*/
void InitializeMpzWithIndex(int arrayLength, MpzWithIndex *array)
{
	for(int i = 0; i < arrayLength; i++)
	{
		mpz_init(array[i].integer);
		array[i].index = 0;
		array[i].stepIndex = 0;
	}
}

/*Destroy MpzWithIndex array*/
void DestroyArray(int arrayLength, MpzWithIndex *array)
{
	for(int i = 0; i < arrayLength; i++)
	{
		mpz_clear(array[i].integer);
	}
	free(array);
}

/*Print MpzWithIndex array*/
void PrintMpzWithIndex(int arrayLength, MpzWithIndex *array)
{
	for(int i = 0 ; i < arrayLength; i++)
	{
		gmp_printf("%4d : %8Zd\n", array[i].index,array[i].integer);
		//gmp_printf("%4d : %.3f\n", array[i].index, Find_Log_MPZ_Double(array[i].integer));
	}
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

