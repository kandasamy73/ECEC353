#ifndef _array_ops_
#define _array_ops_

void init_my_api();
int* my_api_create (int* array1, int* array2, int N, int(*func_ptr)(int a, int b));
int my_api_get_largest_index (int* array);
void my_api_reap (int* array, int* largest);

#endif /* _array_ops_ */
