#include "LZMD.h"

int main()
{
	/*Open file as a binary string*/
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
	
	return 0;
}
