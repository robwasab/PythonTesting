#include "max_32.h"

void insert(i16 * list, i16 index, i16 value)
{
	char save, temp;
	
	save = list[index];
	
	list[index] = value;
	
	index--;
	
	while(index >= 0)
	{
		temp = list[index];
		
		list[index] = save;
		
		if (temp < 0) break;
		
		save = temp;
		
		index--;
	}
}

void organize(i16 * list, i16 size, i16 value)
{
	i16 i;
	
	for (i = size - 1; i >= 0; i--)
	{
		if (list[i] < value)
		{
			insert(list, i, value);
			break;
		}
	}
}
