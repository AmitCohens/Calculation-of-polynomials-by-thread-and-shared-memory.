//
// Created by Amit Cohen on 8/07/2021.
//
#include <stdio.h>

#include <pthread.h>

#include <stdlib.h>

#include <string.h>


#define true 1
#define  false 0
int x;


void calculation(char *);

int amountMiniPoly(const char *);

void completeTheStruct(char *miniPoly, int i);

int searchChar(const char *miniPoly, char a);

void *resultThread(void *index);

void threadFunc();

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
            free(mini);
            exit(EXIT_SUCCESS);
        }
        calculation(polynomial);
    }
}

void calculation(char *polynomial) {//This function divides the string into parts of the polynomial and assembles a structure from each, with coefficient and power .
    char *p,*e;
    x = 0;
    char *temp = (char *) malloc(sizeof(char) * strlen(polynomial));
    strcpy(temp, polynomial);
    p = strtok(polynomial, ",");
    if(searchChar(p,' ')){
        puts("The input is invalid");
        free(temp);
        return;
    }
    e=strtok(NULL, ",");
    if(!searchChar(e,' ')){
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
    threadFunc();
    free(temp);
    free(polyArray);
}

void threadFunc() {//This function sends to the thread the monom for calculation
    pthread_t threadId[size];
    int status, *results, result = 0;
    for (int i = 0; i < size; i++) {
        status = pthread_create(&threadId[i], NULL, resultThread, (void *) &mini[i]);
        if (status != 0) {
            fputs("pthread create failure", stderr);
            free(mini);
            exit(EXIT_FAILURE);
        }
    }
    int i = 0;
    while (i < size) {
        pthread_join(threadId[i], (void **) &results);
        result += *results;
        free(results);
        i++;
    }
    printf("%d\n", result);
    free(mini);
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

void *resultThread(void *index) {//This function returns the result from each thread.
    struct miniPoly i = *((struct miniPoly *) index);
    int *result = (int *) malloc(sizeof(int));
    if (result == NULL) {
        perror("The Allocation failed");
        free(mini);
        exit(EXIT_FAILURE);
    }
    int p = power(x, i.power);
    int e = i.coefficient;
    *result = e * p;
    pthread_exit((void *) result);
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
