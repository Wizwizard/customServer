#include <stdlib.h>
#include <stdio.h>

#define STACK_INIT_SIZE 100

typedef int SElemType;

typedef struct sqStack
{
	SElemType *base;
	SElemType *top;
	int sqStacksize;
} sqStack;

void initStack(sqStack *s);
void push(sqStack*, int);
int pop(sqStack*, int*);
void clearStack(sqStack*);
