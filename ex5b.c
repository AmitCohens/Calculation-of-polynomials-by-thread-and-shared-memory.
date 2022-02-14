//
// Created by Amit Cohen on 8/07/2021.
//
#include <stdio.h>

#include <pthread.h>

#include <stdlib.h>

#include <string.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>


#define true 1
#define  false 0
int x;


void calculation(char *);

int amountMiniPoly(const char *);

void completeTheStruct(char *miniPoly, int i);

int searchChar(const char *miniPoly, char a);

void resultSharedMemory(int index);

void sharedMemoryFunc();

int power(int x0, int x1);

struct miniPoly {
    int coefficient;
    int power;
};

struct miniPoly *mini;
int size;

int main() {
    size = 0;
    char polynomial[512];
    while (true) {
        puts("Enter polynomial and X for calculation!");
        fgets(polynomial, 512, stdin);
        polynomial[strlen(polynomial) - 1] = '\0';
        if (strcmp(polynomial, "done") == 0) {
            exit(EXIT_SUCCESS);
        }
        calculation(polynomial);
    }
}

void calculation(
        char *polynomial) {//This function divides the string into parts of the polynomial and assembles a structure from each, with coefficient and power .
    char *p, *e;
    x = 0;
    char *temp = (char *) malloc(sizeof(char) * strlen(polynomial));
    strcpy(temp, polynomial);
    p = strtok(polynomial, ",");
    if (searchChar(p, ' ')) {
        puts("The input is invalid");
        free(temp);
        return;
    }
    e = strtok(NULL, ",");
    if (!searchChar(e, ' ')) {
        puts("The input is invalid");
        free(temp);
        return;
    }
    x = atoi(e);
    size = amountMiniPoly(p);
    char **polyArray = (char **) malloc(sizeof(char *) * size);
    mini = (struct miniPoly *) malloc(sizeof(struct miniPoly) * size);
    if (polyArray == NULL) {
        perror("The Allocation failed");
        free(mini);
        exit(EXIT_FAILURE);
    }
    if (mini == NULL) {
        perror("The Allocation failed");
        free(polyArray);
        exit(EXIT_FAILURE);
    }
    if (size <= 1)
        polyArray[0] = p;
    else
        polyArray[0] = strtok(p, "+");
    completeTheStruct(polyArray[0], 0);
    p = strtok(temp, ",");
    strtok(p, "+");
    for (int j = 1; j < size; j++)
        polyArray[j] = strtok(NULL, "+");
    for (int i = 1; i < size; i++)
        completeTheStruct(polyArray[i], i);
    sharedMemoryFunc();
    free(polyArray);
    free(temp);
}

void
sharedMemoryFunc() {//This function creates the shared memory, creates a son for each monom, at the end summarizes all the results from the shared memory and returns the final result.
    key_t key;
    int shm_id, result = 0;
    int *SMArray;
    key = ftok("/temp", '!');
    shm_id = shmget(key, size * sizeof(int), IPC_CREAT | IPC_EXCL | 0600);
    if (shm_id == -1) {
        perror("failed open the shared mem");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < size; i++) {
        pid_t son = fork();
        if (son < 0) {
            perror("Failed to create a son process");
            free(mini);
            exit(EXIT_FAILURE);
        }
        if (son == 0)
            resultSharedMemory(i);
    }
    for (int son = 0; son < size; son++)
        wait(NULL);
    SMArray = (int *) shmat(shm_id, NULL, 0);
    if (SMArray < 0) {
        perror("failed \n");
        free(mini);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < size; i++) {
        result += SMArray[i];
    }
    printf("%d\n", result);
    free(mini);
    shmdt(SMArray);
    shmctl(shm_id, IPC_RMID, NULL);
}

int amountMiniPoly(const char *poly) {//This function counts how many monoms there are in the polynomial.
    int count = 0;
    for (int i = 0; poly[i] != '\0'; i++)
        if (poly[i] == '+')
            count++;
    return count + 1;
}

void completeTheStruct(char *miniPoly, int i) {//This function builds the array of structures of the monomers.
    int coefficient;
    int power;
    char *temp = (char *) malloc(sizeof(char) * strlen(miniPoly));
    strcpy(temp, miniPoly);
    int searchX = searchChar(miniPoly, 'x');
    if (searchChar(miniPoly, '^')) {
        strtok(temp, "^");
        power = atoi(strtok(NULL, "^"));
    } else if (searchX) {
        power = 1;
    } else {
        power = 0;
        coefficient = atoi(miniPoly);
    }
    strcpy(temp, miniPoly);
    if (searchX) {
        if (strchr(temp, '*') == NULL) {
            coefficient = 1;
        } else {
            strcpy(temp, miniPoly);
            coefficient = atoi(strtok(temp, "*"));
        }
    }
    mini[i].coefficient = coefficient;
    mini[i].power = power;
    free(temp);
}

int searchChar(const char *miniPoly,char a) {//This function returns true or false, if a particular char is inside the string
    for (int i = 0; miniPoly[i] != '\0'; i++)
        if (miniPoly[i] == a)
            return true;
    return false;
}

void resultSharedMemory(int index) {//This function connects each son into the shared memory, calculates the result of the monom and inserts the result into the shared memory.
    int result;
    key_t key;
    int shm_id;
    int *SMArray;
    key = ftok("/temp", '!');
    shm_id = shmget(key, 0, 0600);
    if (shm_id == -1) {
        perror("failed open the shared mem\n");
        exit(EXIT_FAILURE);
    }
    SMArray = (int *) shmat(shm_id, NULL, 0);
    if (SMArray < 0) {
        perror("failed \n");
        exit(EXIT_FAILURE);
    }
    int p = power(x, mini[index].power);
    int e = mini[index].coefficient;
    result = e * p;
    SMArray[index] = result;
    shmdt(SMArray);
    exit(0);
}

int power(int x0, int x1) {//This function calculates the power of a number.
    int x2 = x0;
    if (x1 == 0)
        return 1;
    if (x1 == 1)
        return x0;
    for (int i = 1; i < x1; i++)
        x0 *= x2;
    return x0;
}
