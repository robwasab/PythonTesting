import os
import math
from byte_generator import gen_binary
from simulate import gen_text
from cStringIO import StringIO
import sys
from random import randint
from random import random

C_FILES = '../C_files/'
INPUT   = '../Input/'
OUTPUT  = '../Output/'
EXEC_NAME = 'main'
PY_OUTPUT_NAME = 'py.out'
DIFF_OUT_NAME = 'diff.out'
C_OUTPUT_NAME = 'test.out'
BIN_INPUT_NAME = 'test.bin'

MAX = int(math.pow(2,12)) - 1
MAX_LEN = int(math.pow(2,16)) - 1

def read_diff():
	ret = ''
	f = open(OUTPUT + DIFF_OUT_NAME, 'r')
	
	while True:
		l = f.readline()
		if l == '':
			break
		ret += l
	return ret

def compile_all():
	cmd = 'gcc %s*.c -o %s'%(C_FILES, C_FILES + EXEC_NAME)
	os.system(cmd)
	
def run_main(binary_file=BIN_INPUT_NAME):
	cmd = C_FILES + EXEC_NAME + ' ' + INPUT + binary_file
	os.system(cmd)
	output_file = binary_file[0:-4] + '.out'
	cmd = 'mv ' + INPUT + output_file + ' ' + OUTPUT  
	os.system(cmd)

def unit_test_ascending(start, size):
	a = range(start, start + size)
	b = range(start + size, start)
	data = a + b
	gen_binary(data)
	run_main()
	gen_text(data, OUTPUT+PY_OUTPUT_NAME)
	
	#diff cmd
	cmd = 'diff ' + OUTPUT + C_OUTPUT_NAME +' ' + OUTPUT + PY_OUTPUT_NAME + ' > ' + OUTPUT + DIFF_OUT_NAME
	os.system(cmd)
	
	diff = read_diff()
	
	if len(diff) > 0:
		print 'Test Failed'
		return (False,diff)
	return (True, diff)

def unit_test_descending(start):
	a = range(start, -1)
	b = range(0, start)
	data = a+b
	
	gen_binary(data)
	run_main()
	gen_text(data, OUTPUT+PY_OUTPUT_NAME)
	
	#diff cmd
	cmd = 'diff ' + OUTPUT + C_OUTPUT_NAME +' ' + OUTPUT + PY_OUTPUT_NAME + ' > ' + OUTPUT + DIFF_OUT_NAME
	os.system(cmd)
	
	diff = read_diff()
	
	if len(diff) > 0:
		print 'Test Failed'
		return (False,diff)
	return (True, diff)

def unit_test_uniform(val, size):
	data = [val] * size
	gen_binary(data)
	run_main()
	gen_text(data, OUTPUT+PY_OUTPUT_NAME)
	
	#diff cmd
	cmd = 'diff ' + OUTPUT + C_OUTPUT_NAME +' ' + OUTPUT + PY_OUTPUT_NAME + ' > ' + OUTPUT + DIFF_OUT_NAME
	os.system(cmd)
	
	diff = read_diff()
	
	if len(diff) > 0:
		print 'Test Failed'
		return (False,diff)
	return (True, diff)
	
def unit_test_random(size):
	data = [int(MAX*random()) for i in xrange(size)]
	gen_binary(data)
	run_main()
	gen_text(data, OUTPUT+PY_OUTPUT_NAME)
	
	#diff cmd
	cmd = 'diff ' + OUTPUT + C_OUTPUT_NAME + ' ' + OUTPUT + PY_OUTPUT_NAME + ' > ' + OUTPUT + DIFF_OUT_NAME
	os.system(cmd)
	
	diff = read_diff()
	
	if len(diff) > 0:
		print 'Test Failed'
		print 'Printing Out Random Data...'
		return (False,diff)
	return (True, diff)


def print_c_output():
	print 'C OUTPUT'
	cmd = 'cat ' + OUTPUT + 'test.out'
	os.system(cmd)

def print_py_output():
	print 'PYTHON OUTPUT'
	cmd = 'cat ' + OUTPUT + PY_OUTPUT_NAME
	os.system(cmd)


def test_ascending_descending():
	compile_all()
	stop = MAX
	for i in range(0,MAX+1):
		print '>%x...'%(i)
		passed, output = unit_test_ascending(0,i)
		if not passed:
			print_py_output()
			print_c_output()
			print 'Ascending from %d to %d' % (0, i)
			print output
			return

		passed, output = unit_test_descending(i)
		if not passed:
			print_py_output()
			print_c_output()
			print 'Descending from %d to %d' % (i, i)
			print output
			return


def test_uniform():
	compile_all()
	for size in range(0, MAX_LEN):
		val = randint(0, MAX)
		print 'size: 0x%x\tval: %d'%(size, val)
		passed, output = unit_test_uniform(val, size)
		if not passed:
			print_py_output()
			print_c_output()
			print 'Unform size %d\tval %d' % (size, val)
			print output
			return

def test_random():
	compile_all()
	for size in range(0, MAX_LEN+1, 0xff):
		print '0x%x'%(size)
		passed, output = unit_test_random(size)
		if not passed:
			print_py_output()
			print_c_output()
			print 'Random size %d' % (size)
			print output
			return
def main():
	#test_ascending_descending()
	#test_uniform()
	test_random()
	
if __name__ == '__main__':
	main()