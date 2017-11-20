#include <stdio.h>

#define INT_DECLARATOR(a)\
    int var_##a##_generated=0;

#define VAR_NAME(a)\
    var_##a##_generated

#define VAR_STRING(a)\
    "var_" #a "_generated"

int main()
{
    INT_DECLARATOR(1)
    
    printf("We print var '" VAR_STRING(1) "' %d\n",VAR_NAME(1));

    return 0;
}

