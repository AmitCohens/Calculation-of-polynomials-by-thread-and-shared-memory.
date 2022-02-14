Calculation of polynomials by thread and shared memory.
Authored by Amit Cohen |Ex5| 8/07/21
315147330

==Description==

This software is divided into two parts, calculating polynomials by thread's and calculating polynomials using shared memory.
-----------------------------------------------------------------------------------------------------------------------------
program DATABASE:
1) x-the The number to be calculated in the polynomial
-----------------------------------------------------------------------------------------------------------------------------
functions that exist in both programs:

1) void calculation(char *)-This function divides the string into parts of the polynomial and assembles a structure from each, with coefficient and power .
2) int amountMiniPoly(const char *)-This function counts how many monoms there are in the polynomial.
3) void completeTheStruct(char *miniPoly, int i)-//This function builds the array of structures of the monoms
4) int searchChar(const char *miniPoly, char a)-This function returns true or false, if a particular char is inside the string
5) int power(int x0, int x1)-This function calculates the power of a number.

Functions that only exist in a program with threads:

1) void threadFunc()- This function sends to the thread the monom for calculation,and return the result.
2) void *resultThread(void *index)- This function returns the result from each thread.

Functions that exist only in a program with shared memory:

1) void sharedMemoryFunc()-This function creates the shared memory, creates a son for each monom, at the end summarizes all the results from the shared memory and returns the final result.
2) void resultSharedMemory(int index)-This function connects each son into the shared memory, calculates the result of the monom and inserts the result into the shared memory.
-----------------------------------------------------------------------------------------------------------------------------
==Program Files==
1) ex5a-thread program
2) ex5b-shared memory
-----------------------------------------------------------------------------------------------------------------------------
==How to compile?==
1)ex5a=gcc ex5a.c -o ex5a -lpthread
2)ex5b= gcc ex5b.c -o ex5b
-----------------------------------------------------------------------------------------------------------------------------
==How to run?==
1)ex5a= ./ex5a
2)ex5b= ./ex5b
-----------------------------------------------------------------------------------------------------------------------------
The change in code between the first and second plan was the part of the division into shared memory / thraeds and the calculation of the final result.
