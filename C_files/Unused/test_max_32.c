#include <stdio.h>
#include <stdlib.h>
#include "max_32.h"

void init_array(i16 * arr, u16 len, i16 val)
{
	--len;
	do
	{
		arr[len] = val;
	} while(len--);
}


int main()
{
	int size;
	scanf("%d", &size);
	int gv_size = 32;
	i16 * greatest_values = (i16*)malloc(sizeof(i16)*gv_size);
	
	init_array(greatest_values, gv_size, -1);
	
	int input;
	for (int i = 0; i < size; i++)
	{
		scanf("%d", &input);
		organize(greatest_values, gv_size, input);
	}
	
	int i;
	for (i = 0; i < gv_size; i++) 
		if (greatest_values[i] > -1) printf("%d\n", greatest_values[i]);

}
