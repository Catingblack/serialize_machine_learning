import os,sys
import numpy as np

#open yuv file (planar 16bits per component 422 files)
#returns an array with 3 arrays (one per component)
def read_yuv16p(filename, width, height):
	f =  open(filename,"r")
	y =  np.fromfile(f, dtype=np.dtype('int16'), count=width * height)
	cb = np.fromfile(f, dtype=np.dtype('int16'), count=width * height / 2)
	cr = np.fromfile(f, dtype=np.dtype('int16'), count=width * height / 2)
	f.close()
	return [y,cb,cr]





if __name__ == '__main__':
	a = read_yuv16p(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))
	print a[0][:20]
