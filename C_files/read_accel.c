#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* P R O G R A M   O V E R V I E W
 *
 * P R O B L E M   S T A T E M E N T 
 *
 * Extract packed, 12-bit, Big-Endian formatted values from a binary file. 
 * Hold onto the 32 greatest values. 
 * Print the last 32 values at the end of the program.
 *
 * C H A L L E N G E S
 *
 * Reading packed Big-Endian data from a binary file, especially when one bye is shared 
 * between two different different values.
 *
 * Printing the last 32 values without using a separate array to constantly retain last
 * read values.
 *
 * S O L U T I O N S
 *
 * P A R S I N G   1 2 - B I T   D A T A
 *
 * Implemented a state machine to read the Big-Endian formatted data. Take for example the 
 * following  sequence of numbers, each number represents a nibble, bytes are 
 * separated by spaces:
 * 12 34 56 78 9A BC...
 *
 * The following sequence represents how one should separate the nibbles:
 * 12 3|4 56 78 9|A BC
 * '|' denotes the splitting of a byte into nibbles
 *
 * There are three states: STATE_A, STATE_B, STATE_C. 
 *
 * STATE_A is to read a byte,left shift it by 4 bits, and save it into a variable. 
 * In the sequence 12 34 56 78... this would be the byte 12. 
 *
 * STATE_B reads a byte and splits it into to two nibbles. The most significant nibble 
 * is shifted down by 4 bits and added to the value saved in STATE_A. One 12-bit value has
 * just been recovered. The least significant nibble is shifted up by 8 bits and 
 * saved for STATE_C.
 *
 * STATE C reads a byte and adds it to the least significant nibble found in STATE_B. The 
 * second 12-bit has been recovered and is printed/sorted. Go back to STATE_A.
 *
 * G R E A T E S T   3 2   V A L U E S
 *
 * Keeping track of the greatest 32 values and keeping them sorted was straightforward. 
 * I used a 32 cell 16-bit array to keep track of the 12-bit values, this was a 
 * performance tradeoff, since 32 nibbles will go unused, but it makes the algorithm simple
 * and straightforward. 
 *
 * L A S T   3 2   V A L U E S
 *
 * I tried really hard not to use another array to keep track of the last 32 values. 
 * Ultimately, I re-used the raw_data buffer used to read from file. Accounting for boundary
 * misalignments, i.e. there being one or two extra bytes at the end of file, resulted in
 * making very special modifications to account for them. This led to an efficiency downgrade
 * since the program reads 3 bytes at a time (slow). Ideally, it would read 32 values
 * at a time, but you cannot detect the extra byte conditions mentioned above. 
 *
 */


/*
SORTED_SIZE defines the size of the array that will carry
the sorted values.

RAW_SIZE defines the size of the byte array that will be used
to read from the binary file. 
48 bytes is 32 12-bit values, or one and one half of 32. 
*/

#define SORTED_SIZE		32
#define RAW_SIZE 		(SORTED_SIZE + SORTED_SIZE/2)

typedef uint8_t		u8;
typedef int8_t		i8;
typedef uint16_t	u16;
typedef int16_t		i16;

/**
 * @brief Helper function for keeping track of the 32 greatest values.
 * 
 * value is copied into the desired index and the element that was previously at that
 * index is copied down into the previous index and so forth. Element zero is forgotten.
 * 
 * @param list Pointer to the sorted values
 * @param index Index to copy value into
 * @param value
 * @return Void.
 */
 
void insert(i16 * list, i16 index, i16 value)
{
	i16 save, temp;
	
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

/**
 * @brief Entry point for keeping track of 32 greatest values. 
 * Finds the index where to put the value wished to be sorted. If the value is bigger 
 * than an element in the list it is inserted into that element's index, and the list 
 * is shifted down towards index zero.
 * 
 * @param list Pointer to the sorted values
 * @param size List's size
 * @param value The value to check
 * @return Void.
 */
 
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

/**
 * @brief Reads raw packed 12-bit data stored in a byte array and either keeps track of 
 * the 32 greatest values or prints out the 12-bit values in order of appearance.
 * 
 * @param output Printing output stream.
 * @param raw Pointer to byte array containing packed 12-bit data
 * @param raw_len Length of byte array
 * @param sorted_list Pointer to list containing sorted data. If NULL, function prints
 * values in order of appearance to output.
 * @param sort_len Length of sorted_list. If sorted_list is NULL, if sort_len is equal to
 * 1, the function skips printing the first value read in raw. 
 */

void sort(FILE * output, u8 * raw, u16 raw_len, i16 * sorted_list, i16 sort_len)
{
	typedef enum {STATE_A, STATE_B, STATE_C} State;
	
	State state = STATE_A;
	
	i16 value = 0;
	
	u16 i;
	
	for (i = 0; i < raw_len; i++)
	{
		switch(state)
		{
			case STATE_A:
			
				value = raw[i] << 4;
					
				state = STATE_B;
				
				break;
				
			case STATE_B:
			
				value += ((0xf0 & raw[i]) >> 4);
				
				if (sorted_list) organize(sorted_list, sort_len, value);
				
				else {
					if (sort_len > 0) sort_len -= 1;
					
					else fprintf(output, "%d\n", value); 
				}
				
				value = (raw[i] & 0x0f) << 8;
				
				state = STATE_C;
				
				break;
			
			case STATE_C:
			
				value += raw[i];

				if (sorted_list) organize(sorted_list, sort_len, value);

				else fprintf(output, "%d\n", value); 
				
				state = STATE_A;

				break;
		}
	}
}

void init_array(i16 * arr, u16 len, i16 val)
{
	--len;
	do
	{
		arr[len] = val;
	} while(len--);
}

void print_usage(char * argv[])
{
	printf("usage: %s binary.bin\n", argv[0]);
}

char change_ext(char * in_name)
{
	u16 i, k;
	char ext[] = "out";
	
	int len = 0;
	while (in_name[len++] != '\0');
	
	for (i = len - 1; i >= 0; i--)
	{ 	
		if (in_name[i] == '.')
		{
			i += 1;
			for (k = 0; ext[k] != '\0' && in_name[k + i] != '\0'; k++)
			{
				in_name[i+k] = ext[k];
			}
			if (k != sizeof(ext)-1) return -1;
			else return 0; 
		}
	}
	return -1;
}

int main(int argc, char * argv[])
{	
	// Data Initialization and Variable Declarations 
	
	i16  sorted_values[SORTED_SIZE];
	u8   raw_data     [RAW_SIZE] = {0};
	u8   holder       [3]        = {0};
	u8 * pntr                    = raw_data;
	
	init_array(sorted_values, 32, -1);

	// Check the program arguments...
	if      (argc < 2) { print_usage(argv); exit(-1); }
	else if (2 < argc) { printf("Too many arguments...\n"); print_usage(argv); exit(-1); }
	
	char * name = argv[1];
	
	// Open the input an output files...
	FILE * file = fopen(name, "r");

	if (!file) { printf("Cannot open file...\n"); exit(-1); }

	// Attempts to change the input file's extension to .out 
	if ( change_ext(name) ) { printf("Invalid file extension...\n"); print_usage(argv); exit(-1); }
	
	FILE * output = fopen(name, "w");
	
	// Loop Variables num_read is for the number of bytes read at a time from
	// input stream
	u8 num_read;
	
	u16 total_count = 0;
	
	while (1) 
	{
		// Bad performance because it reads 3 bytes at a time
		// Read the data into a temporary buffer called holder
		num_read = fread(holder, sizeof(u8), sizeof(holder), file);
		
		// End of file condition
		if (num_read < sizeof(holder)) { break; }
		
		// If the program was able to read 3 bytes copy the data
		// into the byte buffer
		pntr[0] = holder[0]; pntr[1] = holder[1]; pntr[2] = holder[2];
		
		total_count += num_read;
		
		pntr = raw_data + (total_count % sizeof(raw_data));
		
		// Once you read sizeof(raw_data) bytes process the data
		if (total_count % sizeof(raw_data) == 0)
		{
			sort(output, raw_data, sizeof(raw_data), sorted_values, SORTED_SIZE);
		}
	}
	
	// Sort the remaining values in the raw_data array since they havent been processed
	// by sort
	
	sort(output, raw_data, pntr - raw_data, sorted_values, SORTED_SIZE);
	
	// sort wont do anything if num_read is 1 
	sort(output, holder, num_read, sorted_values, SORTED_SIZE);
	
	fprintf(output, "--Sorted Max 32 Values--\n");
	
	int i;
	for (i = 0; i < SORTED_SIZE; i++) 
		if (sorted_values[i] > -1) fprintf(output, "%d\n", sorted_values[i]);
	
	fprintf(output, "--Last 32 Values--\n");
	
	/* This statement prints out old data that was not overwritten.
	The only instance in which we will not need to print saved, old data
	is when there wasn't any saved data in the first place, which is when
	total_count was less than the size of the raw array */
	
	if (total_count >= RAW_SIZE) 
	{
		/*
		Below you'll see the statement: (num_read > 0) ? num_read - 1 : 0
		Which is a hack. 
		if num read is 2, then it'll return 1, if num_read is 1, then 0. 
		num_read won't be greater than 2.
		
		The goal is: if num read is 2, then skip the first number printed, 
		recall that sort is responsible for not only sorting, but also printing the raw_data.
		To tell sort to skip the first print, I re-used the parameter sort_len. Since 
		sorting is disabled when the sorting pointer is NULL.
		*/
		
		sort(output, pntr, RAW_SIZE - (pntr - raw_data), NULL, (num_read > 0) ? num_read - 1 : 0);
	}
	sort(output, raw_data, (pntr - raw_data), NULL, 0);
	
	if (num_read) sort(output, holder, num_read, NULL, 0);
	
	fprintf(output, "\n");
	fclose(file);
	fclose(output);
}
