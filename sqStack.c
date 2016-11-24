/*
#include <stdio.h>
#include <stdlib.h>

#define STACK_INIT_SIZE 100
#define STACKINCREMENT 2

typedef int SElemType;

typedef struct sqStack
{
	SElemType *base;
	SElemType *top;
	int sqStacksize;
}sqStack;

*/
#include "sqStack.h"

void initStack(sqStack *s)
{
	(*s).base = (SElemType *) malloc (STACK_INIT_SIZE * sizeof(SElemType));
	(*s).top = (*s).base;
	(*s).sqStacksize = STACK_INIT_SIZE;
}

void clearStack(sqStack *s)
{
	(*s).top = (*s).base;
}

void push(sqStack *s, SElemType e)
{
	//需要判断是否栈满了
	
	*((*s).top)++ = e;
}

int pop(sqStack *s, SElemType *e)
{
	if((*s).top == (*s).base)
		return 0;
	
	*e = *--(*s).top;
	return 1;
}
/*
int main()
{
	sqStack s;
	initStack(&s);
	
	int j ;
	for(j = 0; j < 5; j ++)
		push(&s, j);

	int num;
	printf("push completed\n");
	
	while(pop(&s, &num))
	{
		printf("%d ", num);
	}
	printf("\n");
}
*/
