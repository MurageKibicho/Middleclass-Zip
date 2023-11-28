#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include "Q.h"
int GetFileSize(char *fileName){FILE *fp = fopen(fileName, "rb");assert(fp != NULL);fseek(fp, 0L, SEEK_END);int currentFileSize = ftell(fp);rewind(fp);fclose(fp);return currentFileSize;}



void PrintMatrix(int matrixLength,int *matrix)
{
	for(int i = 0; i < matrixLength; i+=8)
	{
		printf("{ ");
		for(int j = 0; j < 7; j++)
		{
			printf("%3d,",matrix[i+j]);
		}
		printf("%3d}, \n",matrix[i+7]);
	}
	printf("\n");
}

void Print2DMatrix(int rows, int columns, int **matrix)
{
	for(int i = 0; i < rows; i++)
	{
		for(int j = 0; j < columns; j++)
		{
			printf("%3d, ",matrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void SetMatrix(int rows, int columns, int **matrix, int *array){for(int i = 0 ; i < rows; i++){for(int j = 0; j < columns; j++){matrix[i][j] = *(array + i * columns + j);}}}
int **Create2dMatrix(int rows, int columns){int **matrix= malloc(rows * sizeof(int*));for(int i = 0 ; i < rows; i++){matrix[i]=calloc(columns, sizeof(int));}return matrix;}
int Reset2dMatrix(int rows, int columns,int **matrix){for(int i = 0 ; i < rows; i++){memset(matrix[i], 0, sizeof matrix[i][0] * columns); }}
void Destroy2dMatrix(int rows, int **matrix){for(int i = 0 ; i < rows; i++){free(matrix[i]);}free(matrix);}
int Compare2dMatrix(int rows, int columns, int **matrix0, int **matrix1){int matchCount = 0;for(int i = 0 ; i < rows; i++){for(int j = 0; j < columns; j++){if(matrix0[i][j] == matrix1[i][j]){matchCount += 1;}}}return matchCount;}
void Copy2dMatrix(int rows, int columns, int **matrixSource, int **matrixDestination){for(int i = 0 ; i < rows; i++){for(int j = 0; j < columns; j++){matrixDestination[i][j] = matrixSource[i][j];}}}
void UnsetMatrix(int rows, int columns, int **matrix, int *array){for(int i = 0 ; i < rows; i++){for(int j = 0; j < columns; j++){*(array + i * columns + j) = matrix[i][j] ;}}}
int main()
{
	char *fileName = "build.jpg";
	int fileSize = GetFileSize(fileName);
	int *fileContent  = calloc(fileSize, sizeof(int)); 
	int c = 0;int sampleIndex = 0;FILE *fp = fopen(fileName, "rb");assert(fp != NULL);while((c = fgetc(fp)) != EOF){fileContent[sampleIndex] = c;sampleIndex+= 1;}fclose(fp);
	
	
	int matchCount = 0;
	int rows = 100;
	int columns = 1000;
	int actions = 2;
	int episodes = 500;
	float alpha = 0.9; // Learning rate
	float gamma = 0.1; // Discount factor
	float epsilon = 0.9; // Epsilon-greedy parameter
	int penalty_threshold = 128; // Threshold to penalize near-zero values
	int **qTable = Create2dMatrix(rows *columns, 2);
	
	int counter = 0;
	int **matrix     = Create2dMatrix(rows, columns);
	int **matrixCopy = Create2dMatrix(rows, columns);
	int **matrixResult = Create2dMatrix(rows, columns);
	int index = 0;
	int totalSize = fileSize / (rows*columns);
	int difference = 1;
	for(int i = 0; i < totalSize; i++)
	{
		assert(index+(rows *columns * i) < fileSize);
		Reset2dMatrix(rows *columns, 2, qTable);
		SetMatrix(rows, columns, matrix, fileContent+index+(rows *columns * i));
		SetMatrix(rows, columns, matrixCopy, fileContent+index+(rows *columns * i));
		qLearning(rows, columns, actions, episodes, alpha, gamma, epsilon, penalty_threshold, matrix, qTable);
		ReducePath(rows, columns, qTable, matrix,difference);

		Copy2dMatrix(rows, columns, matrix, matrixResult);

		Reset2dMatrix(rows *columns, 2, qTable);
		qLearning(rows, columns, actions, episodes, alpha, gamma, epsilon, penalty_threshold, matrix, qTable);
		IncreasePath(rows, columns, qTable, matrix,difference);	
		matchCount = Compare2dMatrix(rows, columns, matrix, matrixCopy);	
		//printf("%d / %d\n",counter,totalSize);
		if(matchCount == (rows*columns))
		{
			UnsetMatrix(rows, columns, matrixResult, fileContent+index+(rows *columns * i));
			counter += 1;
		}
		else
		{
			Print2DMatrix(rows , columns, matrixCopy);		
		}
	
	}
	printf("Success! %d %d\n",counter,totalSize);
	FILE *fr = fopen("result.lzql", "wb");
	unsigned char byte = 0;
	for(int i = 0; i < fileSize; i++)
	{
		assert(fileContent[i] > -1);
		assert(fileContent[i] < 256);
		byte = (unsigned char) fileContent[i];
		fwrite(&byte,sizeof(unsigned char),1, fr);
	}
	fclose(fr);	
	//Print2DMatrix(rows, columns, matrix);
	//Print2DMatrix(rows, columns, matrixCopy);
	//printBestPath(rows, columns, qTable, matrix);

	Destroy2dMatrix(rows * columns, qTable);
	Destroy2dMatrix(rows, matrix);
	Destroy2dMatrix(rows, matrixCopy);
	Destroy2dMatrix(rows, matrixResult);
	free(fileContent);
	return 0;
}
