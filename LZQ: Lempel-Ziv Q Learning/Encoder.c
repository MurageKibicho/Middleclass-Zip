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

int Compare2dMatrix(int rows, int columns, int **matrix0, int **matrix1)
{
	int matchCount = 0;
	for(int i = 0 ; i < rows; i++){for(int j = 0; j < columns; j++){if(matrix0[i][j] == matrix1[i][j]){matchCount += 1;}}}
	return matchCount;
}
int main()
{
	char *fileName = "build.jpg";
	int fileSize = GetFileSize(fileName);
	int *fileContent  = calloc(fileSize, sizeof(int)); 
	int c = 0;int sampleIndex = 0;FILE *fp = fopen(fileName, "rb");assert(fp != NULL);while((c = fgetc(fp)) != EOF){fileContent[sampleIndex] = c;sampleIndex+= 1;}fclose(fp);
	
	
	int matchCount = 0;
	int rows = 8;
	int columns = 8;
	int actions = 2;
	int episodes = 1000;
	float alpha = 0.1; // Learning rate
	float gamma = 0.9; // Discount factor
	float epsilon = 0.1; // Epsilon-greedy parameter
	int penalty_threshold = 20; // Threshold to penalize near-zero values
	int **qTable = Create2dMatrix(rows *columns, 2);
	
	int counter = 0;
	int **matrix     = Create2dMatrix(rows, columns);
	int **matrixCopy = Create2dMatrix(rows, columns);
	int index = 0;
	int totalSize = fileSize / 64;
	for(int i = 0; i < totalSize; i++)
	{
		assert(index+(rows *columns * i) < fileSize);
		Reset2dMatrix(rows *columns, 2, qTable);
		SetMatrix(rows, columns, matrix, fileContent+index+(rows *columns * i));
		SetMatrix(rows, columns, matrixCopy, fileContent+index+(rows *columns * i));
		qLearning(rows, columns, actions, episodes, alpha, gamma, epsilon, penalty_threshold, matrix, qTable);
		ReducePath(rows, columns, qTable, matrix);
		PrintBestPath(rows, columns, qTable, matrix);	
		Reset2dMatrix(rows *columns, 2, qTable);
		qLearning(rows, columns, actions, episodes, alpha, gamma, epsilon, penalty_threshold, matrix, qTable);
	
		IncreasePath(rows, columns, qTable, matrix);	
		matchCount = Compare2dMatrix(rows, columns, matrix, matrixCopy);	
		printf("%d\n",matchCount);
		if(matchCount == 64){
		
		counter += 1;}
	
	}
	printf("%d %d\n",counter,totalSize);
	
	//Print2DMatrix(rows, columns, matrix);
	//Print2DMatrix(rows, columns, matrixCopy);
	//printBestPath(rows, columns, qTable, matrix);

	Destroy2dMatrix(rows * columns, qTable);
	Destroy2dMatrix(rows, matrix);
	Destroy2dMatrix(rows, matrixCopy);
	free(fileContent);
	return 0;
}
