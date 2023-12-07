#include <stdio.h>
#include <math.h>

#include "stack.h"
#include "debug.h"


int main()
{
    INIT_LOG;

    Stack stk = {};
    StackInit(&stk);



    for (size_t i = 0; i < 15; i++)
    {
        Push(&stk, 10);
    }



    StackDtor(&stk);




    CLOSE_LOG;
    return 0;
}
