#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <gmp.h>

int GCD(int num, int den)
{
	if(num == den){return den;}if(num == 0){return den;}if(den == 0){return num;}if(num > den){return GCD(num-den,den);}
	return GCD(num,den-num);
}

//https://www-jstor-org.yale.idm.oclc.org/stable/2028335?seq=3#metadata_info_tab_contents

//https://math.stackexchange.com/questions/2053144/solving-ax-equiv-c-pmod-b-efficiently-when-a-b-are-not-coprime/2053174#2053174
//https://math.stackexchange.com/questions/67969/linear-diophantine-equation-100x-23y-19/68021
//Solves ax = b (mod) i.e 34x cong 51 mod 85 === 34x-51 = 85y
int FractionalEuclidean(int a,int b,int mod)
{	
	//Handle case when a is negative
	if(a < 0)
	{
		a *= -1;
		b *= -1;
	}
	int gcd = GCD(a,mod);
	
	//Handle Case when a and mod are not coprime
	if(gcd != 1)
	{
		printf("(Not coprime): %dx = %d mod %d\n",a,b,mod);
	}
	assert(gcd == 1);
	int previous[] = {mod,0};
	int current[] = {a, b};
	int count = 0;
	while(current[0] != 1)
	{
		int remainder = previous[0] % current[0];
		int divisor = previous[0] / current[0];
		int right = previous[1] - (divisor * current[1]);
		right %= mod;
		if(right < 0)
		{
			right += mod;
		}
		previous[0] = current[0];
		previous[1] = current[1];
		current[0] = remainder;
		current[1] = right;
		count++;
		if(current[0] == 0)
		{
			//printf("Did not handle this");
			return -1;
		}
	}
	if(b < 0)
	{
		b = mod + b;
		current[1] = b;
	}
	//printf("%dx = %d\n", current[0], current[1]);
	return current[1];
}
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

void PrintMpzWithIndex(int arrayLength, MpzWithIndex *array)
{
	for(int i = 0 ; i < arrayLength; i++)
	{
		gmp_printf("%4d : %8Zd\n", array[i].index,array[i].integer);
		//gmp_printf("%4d : %.3f\n", array[i].index, Find_Log_MPZ_Double(array[i].integer));
	}
}

void DestroyArray(int arrayLength, MpzWithIndex *array)
{
	for(int i = 0; i < arrayLength; i++)
	{
		mpz_clear(array[i].integer);
	}
	free(array);
}

MpzWithIndex *CreateArray(int bitSize, int binaryLength, int *binary, int *arrayLength)
{
	*arrayLength = binaryLength / bitSize;
	MpzWithIndex *array = malloc(*arrayLength * sizeof(MpzWithIndex));
	InitializeMpzWithIndex(*arrayLength, array);
	int currentBitIndex = 0;
	int currentIntegerIndex = 0;
	int i = 0;
	for(i = 0; i < binaryLength; i += bitSize, currentIntegerIndex += 1)
	{
		currentBitIndex = bitSize-1;
		if(currentIntegerIndex >= *arrayLength){printf("Overflow\n");*arrayLength -= 1;break;}
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

void UnmakeRandom(int randomSeed, int randomStep, int leftBitSize, int leftLength, MpzWithIndex *leftArray)
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
		mpz_sub(leftArray[i].integer, leftArray[i].integer, temporary0);
		mpz_mod(leftArray[i].integer, leftArray[i].integer, temporary2);
	}
	mpz_clear(temporary0);
	mpz_clear(temporary1);mpz_clear(temporary2);
	gmp_randclear(state);	
}

void UnmixArrays2(int leftLength, MpzWithIndex *leftArray, int rightBitSize, int rightLength, MpzWithIndex *rightArray, int stepSize, MpzWithIndex *lookupTable , int spaceFound)
{
	qsort(leftArray, leftLength, sizeof(MpzWithIndex), MpzWithIndexCompareMpzIntegerOpposite);
	qsort(rightArray, rightLength, sizeof(MpzWithIndex), MpzWithIndexCompareMpzIntegerOpposite);
	printf("%d\n",spaceFound);
	//PrintMpzWithIndex(rightLength, rightArray);	
	mpz_t maximum; mpz_init(maximum);
	mpz_t temporary0; mpz_init(temporary0);
	mpz_setbit(maximum, spaceFound);
	for(int i = 0; i < stepSize; i++)
	{
		mpz_add(lookupTable[i].integer, lookupTable[i].integer,maximum);
	}
	int lookupIndex = 0;
	for(int i = 0; i < rightLength; i++)
	{
		int comparison0 = mpz_cmp(lookupTable[lookupIndex].integer, rightArray[i].integer);
		while(comparison0 < 0)
		{
			lookupIndex += 1;
			if(lookupIndex == stepSize){i = rightLength; break;}
			comparison0 = mpz_cmp(lookupTable[lookupIndex].integer, rightArray[i].integer);	
		}
		mpz_sub(temporary0, lookupTable[lookupIndex].integer,maximum);
		printf("%d %.3f %.3f %.3f\n",lookupIndex,Find_Log_MPZ_Double(temporary0),Find_Log_MPZ_Double(rightArray[i].integer),Find_Log_MPZ_Double(lookupTable[lookupIndex].integer) );	
	}
	mpz_clear(maximum);mpz_clear(temporary0);
	/*int currentRightIndex = 0;	
	for( ; currentRightIndex < rightLength; currentRightIndex++)
		{
			int comparison0 = mpz_cmp(lookupTable[i].integer, rightArray[currentRightIndex].integer);
			if(comparison0 < 0){break;}
			int comparison1 = mpz_cmp(temporary0, rightArray[currentRightIndex].integer);
			
			printf("%d %d %.3f %.3f %.3f\n",comparison0,comparison1, Find_Log_MPZ_Double(lookupTable[i].integer),Find_Log_MPZ_Double(rightArray[currentRightIndex].integer),Find_Log_MPZ_Double(temporary0));
		}
	//break;
	}*/

	/*mpz_t maximum; mpz_init(maximum);	
	mpz_t findSpace; mpz_init(findSpace);	
	mpz_set_ui(maximum,0);mpz_setbit(maximum, spaceFound);

	mpz_t difference; mpz_init(difference);
	mpz_t previousMaximum; mpz_init(previousMaximum);
	int currentRightIndex = 0;	
	for(int i = 0; i < stepSize; i++)
	{
		if(currentRightIndex >= rightLength){break;}
		mpz_set(previousMaximum, findSpace);
		mpz_add(findSpace, findSpace, maximum);
		int comparison = mpz_cmp(findSpace, rightArray[currentRightIndex].integer);
		
	}
	
	mpz_clear(findSpace);
	mpz_clear(maximum);
	mpz_clear(difference);
	mpz_clear(previousMaximum);*/
}


void UnmixArrays(int leftLength, MpzWithIndex *leftArray, int rightBitSize, int rightLength, MpzWithIndex *rightArray, int stepSize, MpzWithIndex *lookupTable , int spaceFound)
{
	for(int i = 0; i < stepSize; i++)
	{
		gmp_printf("%Zd %.3f\n",lookupTable[i].integer,Find_Log_MPZ_Double(lookupTable[i].integer));

	}

}

int main()
{
	int leftBitSize = 0;
	int leftRemainder = 0;

	int rightBitSize = 0;
	int rightRemainder = 0;
	
	int spaceFound = 0;
	int stepBits = 0;
	int stepSize = 0;
	
	char *fileName = "m.kk";
	int fileSize = GetFileSize(fileName);
	FILE *fp = fopen(fileName, "rb");assert(fp != NULL);

	unsigned char byte = 0;
	fread(&byte, sizeof(unsigned char), 1, fp);
	assert(byte == 'M');
	fread(&byte, sizeof(unsigned char), 1, fp);
	assert(byte == 'K');
	fread(&byte, sizeof(unsigned char), 1, fp);
	assert(byte == 0);
	fread(&byte, sizeof(unsigned char), 1, fp);
	leftBitSize = (int) byte;
	fread(&byte, sizeof(unsigned char), 1, fp);
	rightBitSize = leftBitSize + (int) byte;
	fread(&byte, sizeof(unsigned char), 1, fp);
	leftRemainder = (int) byte;
	fread(&byte, sizeof(unsigned char), 1, fp);
	rightRemainder = (int) byte;
	fread(&byte, sizeof(unsigned char), 1, fp);
	spaceFound = (int) byte;
	fread(&byte, sizeof(unsigned char), 1, fp);
	stepBits = (int) byte;
	stepSize = 1 << stepBits;
	
	int *binary = calloc((fileSize) * 8, sizeof(int));
	int c = 0;int sampleIndex = 0;while((c = fgetc(fp)) != EOF){AddBinary(c, binary, sampleIndex * 8);sampleIndex+= 1;}fclose(fp);	
	
	int leftArrayLength = 0;
	int leftBinaryLength = 4762*leftBitSize;
	MpzWithIndex *leftArray  = CreateArray(leftBitSize, leftBinaryLength, binary, &leftArrayLength);assert(leftArrayLength > -1);
	
	int seed = 8568;
	MpzWithIndex *lookupTable = GenerateMixLookup(leftBitSize, seed, stepSize);
	
	//int rightArrayLength = ((fileSize-9) * 8)- (4762*leftBitSize);

	
	int rightArrayLength = 0;
	int rightBinaryLength = ((fileSize) * 8)- leftBinaryLength;
	MpzWithIndex *rightArray = CreateArray(leftBitSize, rightBinaryLength, binary + leftBinaryLength, &rightArrayLength);assert(rightArrayLength > -1);
	//PrintMpzWithIndex(rightArrayLength, rightArray);	
	int randomStep = 100;
	int randomSeed = 10;

	UnmixArrays(leftArrayLength, leftArray, rightBitSize, rightArrayLength, rightArray, stepSize, lookupTable,spaceFound);
	UnmakeRandom(randomSeed, randomStep, leftBitSize, leftArrayLength, leftArray);	
	//PrintMpzWithIndex(leftArrayLength, leftArray);	
	DestroyArray(stepSize, lookupTable);
	DestroyArray(leftArrayLength, leftArray);
	DestroyArray(rightArrayLength, rightArray);
	free(binary);
	return 0;
}
/*
4629 : 51160720
4630 : 55033824
4631 :  7423446
4632 :  6174160
4633 : 45386603
4634 : 49418439
4635 :  9161432
4636 : 36374117
4637 : 66668017
4638 : 31083632
4639 : 57951231

0 37.688 22.082	 | 	0 37.688 37.688

0 37.688 25.950	 | 	0 37.688 37.689

1 36.502 24.359	 | 	1 36.502 36.503

2 37.132 23.631	 | 	2 37.132 37.132
*/
