import random
import os
import math
#test code

def test_same_numbers(length, number):
	data = [number]*length;
	
	data_file = open('input.txt', 'w')
	
	data_file.write("%d\n"%(length))
	
	for d in data:
		data_file.write('%d\n'%(d))
	
	data_file.close()
	
	test_file = open('test.txt', 'w')
	
	if length > 32:
		length = 32
		
	for i in range(0, length):
		test_file.write('%d\n'%(number))
		
	test_file.close()
	
	os.system('cat input.txt | ./a.out > output.txt')

	os.system('diff test.txt output.txt > diff.txt')
	
	diff_file = open('diff.txt', 'r')
	
	os.system('cat diff.txt')
	
	if diff_file.readline() != '':
		print 'NO PASS...'
		return False
	else:
		print 'PASS!'
		return True

os.system('gcc test_max_32.c max_32.c')
	
for value in range(0, int(math.pow(2,11))):
	print value
	test_same_numbers(value, 420)
	
	