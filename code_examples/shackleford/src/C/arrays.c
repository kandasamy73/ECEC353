#include <stdio.h>

int main (int argc, char** argv)
{
    void* void_pointer = NULL;
    char char_array[] = "Hello World";
    float float_array[] = {12.34, 5.67, 8.90, 0.0, 3.12345};
    int int_array[] = {11, 22, 33, 44, 55, 66, 77};


    printf("     char_array: %p\n", char_array);
    printf(" &char_array[0]: %p\n", &char_array[0]);
    printf(" &char_array[2]: %p\n\n", &char_array[2]);

    printf("    float_array: %p\n", float_array);
    printf("&float_array[0]: %p\n", &float_array[0]);
    printf("&float_array[2]: %p\n\n", &float_array[2]);

    printf("      int_array: %p\n", int_array);
    printf("  &int_array[0]: %p\n", &int_array[0]);
    printf("  &int_array[2]: %p\n\n", &int_array[2]);

    printf(" (char_array+2): %p\n", (char_array+2));
    printf("(float_array+2): %p\n", (float_array+2));
    printf("  (int_array+2): %p\n\n", (int_array+2));

    void_pointer = (void*)int_array;

    printf("(void_pointer+2): %p\n\n", (void_pointer+2));

    printf("mystery1: %i\n", *(int*)(void_pointer+0));
    printf("mystery2: %i\n", *(int*)(void_pointer+3));
    printf("mystery3: %i\n", *(int*)(void_pointer+4));

    return 0;
}
