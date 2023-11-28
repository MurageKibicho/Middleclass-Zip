#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void qLearning(int rows, int cols, int actions, int episodes, float alpha, float gamma, float epsilon, int penalty_threshold, int **matrix, int **qTable) {
    srand(98875);

    int getState(int x, int y) {return x * cols + y;}

    int chooseAction(int state, float epsilon, int actions, int **qTable) {
        if ((double)rand() / RAND_MAX < epsilon) {return rand() % actions; } else {if (qTable[state][0] > qTable[state][1]) {return 0; /* Move right*/}else{return 1; /*Move down*/}}}

    int getReward(int x, int y, int penalty_threshold, int **matrix) {
        if (matrix[x][y] <= penalty_threshold) {return -1; /* Apply penalty*/} else {return matrix[x][y]; /*Normal reward for other cells*/}}
    for (int i = 0; i < episodes; ++i) {
        int currentState = getState(0, 0);

        while (currentState != getState(rows - 1, cols - 1)) {
            int action = chooseAction(currentState, epsilon, actions, qTable);
            int nextState;if (action == 0) { nextState = currentState + 1; } else { nextState = currentState + cols;}

            if (nextState / cols >= rows || nextState % cols >= cols) { break; /* Prevent moving outside the matrix bounds*/}
            int reward = getReward(nextState / cols, nextState % cols, penalty_threshold, matrix);
            int maxNextAction = (qTable[nextState][0] > qTable[nextState][1]) ? 0 : 1;
            qTable[currentState][action] += alpha * (reward + gamma * qTable[nextState][maxNextAction] - qTable[currentState][action]);
            currentState = nextState;
        }
    }
}


void ReducePath(int rows, int cols, int **qTable, int **matrix, int difference) {
    int getState(int x, int y, int cols) {
        return x * cols + y;
    }

    int current = getState(0, 0, cols);

    while (current != getState(rows - 1, cols - 1, cols)) {
        if (current >= rows * cols) {
            break;
        }

        int bestAction = (qTable[current][0] > qTable[current][1]) ? 0 : 1;

        int x = current / cols;
        int y = current % cols;

        if (bestAction == 0 && y < cols - 1) { // Move right
            current++;
        } else if (bestAction == 1 && x < rows - 1) { // Move down
            current += cols;
        } else {
            break; // Break if attempting to move outside bounds
        }

        x = current / cols;
        y = current % cols;

        if (x >= 0 && x < rows && y >= 0 && y < cols) {
            // Perform operations on matrix within bounds
             if(matrix[x][y]-difference < 0){break;}
             matrix[x][y] -= difference;

        }
    }
}

void IncreasePath(int rows, int cols, int **qTable, int **matrix, int difference) {
    int getState(int x, int y, int cols) {
        return x * cols + y;
    }

    int current = getState(0, 0, cols);

    while (current != getState(rows - 1, cols - 1, cols)) {
        if (current >= rows * cols) {
            break;
        }

        int bestAction = (qTable[current][0] > qTable[current][1]) ? 0 : 1;

        int x = current / cols;
        int y = current % cols;

        if (bestAction == 0 && y < cols - 1) { // Move right
            current++;
        } else if (bestAction == 1 && x < rows - 1) { // Move down
            current += cols;
        } else {
            break; // Break if attempting to move outside bounds
        }

        x = current / cols;
        y = current % cols;

        if (x >= 0 && x < rows && y >= 0 && y < cols) {
            // Perform operations on matrix within bounds
             if(matrix[x][y]+difference > 255){break;}
             matrix[x][y] += difference;
        }
    }
}

void PrintBestPath(int rows, int cols, int **qTable, int **matrix) {
    int getState(int x, int y, int cols) {
        return x * cols + y;
    }

    int current = getState(0, 0, cols);

    while (current != getState(rows - 1, cols - 1, cols)) {
        if (current >= rows * cols) {
            break;
        }

        int bestAction = (qTable[current][0] > qTable[current][1]) ? 0 : 1;

        int x = current / cols;
        int y = current % cols;

        if (bestAction == 0 && y < cols - 1) { // Move right
            current++;
        } else if (bestAction == 1 && x < rows - 1) { // Move down
            current += cols;
        } else {
            break; // Break if attempting to move outside bounds
        }

        x = current / cols;
        y = current % cols;

        if (x >= 0 && x < rows && y >= 0 && y < cols) {
            // Perform operations on matrix within bounds
		//printf("%3d,", matrix[x][y]);
        }
    }
    //printf("\n");
}


