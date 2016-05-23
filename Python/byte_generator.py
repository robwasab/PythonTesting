import bitstring

#val has the lower address in memory, opt_val is the next address in memory	
def write12bit(file, val, opt_val = -1):
	if opt_val < 0:
		s = bitstring.pack('uintbe:16', val<<4)
	else:
		s = bitstring.pack('uintbe:24', (val<<12) + opt_val)
	file.write(s.tobytes())

def gen_binary(ar, name = '../Input/test.bin'):
	output = open(name, 'w')	
	for i in range(0,len(ar),2):
		if i + 1 >= len(ar):
			write12bit(output, ar[i])
		else:
			write12bit(output, ar[i], ar[i+1])		
		
def test():
	gen_binary(data)
	
if __name__ == '__main__':
	test()