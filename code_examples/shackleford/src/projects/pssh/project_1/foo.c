#include <stdio.h>

int main (int argc, char** argv)
{
    unsigned int i;

    for (i=0; i<argc; i++)
        printf ("foo: argv[%i]: %s\n", i, argv[i]);

    return 0;
}
