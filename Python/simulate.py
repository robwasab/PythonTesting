import operator
import copy

def greatest(data, size = 32):
	ret = []
	for i in range(0, size):
		if len(data) > 0:
			index, value = max(enumerate(data),key=operator.itemgetter(1))
			ret.append(value)
			del data[index]
	return ret

def gen_text(data, name = 'py_gen.out', size = 32):

	out = open(name, 'w')
	last = copy.copy(data)
	last = last[-size:]
	
	gdata = greatest(data,size)
	
	out.write('--Sorted Max %d Values--\n'%(size))
	for i in range(len(gdata)-1, -1, -1):
		out.write('%d\n'%(gdata[i])) 
	
	out.write('--Last %d Values--\n'%(size))
	
	for l in last:
		out.write('%d\n'%(l))
	
	out.write('\n')
