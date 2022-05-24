import numpy as np

def get_ae_image(img_ref, img):
	err_img = [np.abs(img_ref[i] - img[i]) for i in range(len(img_ref))]
	return err_img

def get_pae_image(img_ref, img):
	err_img = get_ae_image(img_ref, img)
	return [max(err_comp) for err_comp in err_img]

if __name__ == '__main__':
	from image_read import *
	a = read_yuv16p(sys.argv[1], int(sys.argv[3]), int(sys.argv[4]))
	b = read_yuv16p(sys.argv[2], int(sys.argv[3]), int(sys.argv[4]))
	print get_pae_image(a,b)
