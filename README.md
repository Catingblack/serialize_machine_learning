# JPEG XS Reference Software #

## How to build ##

- For Visual Studio 2015, a project setting can be found in the
  build/vs_2015 directory.

- For Linux, use the following command line:

  `make -C build`
  
  or
  
  `make -C build DEBUG=1`
  
  for a debug build.

## How to run the encoder ##

The encoder currently supports three component ppm files or planar yuv
files (one plane of y, one plane of u, one plane of v) for 9 to 16bpp,
or 8bpp yuv files in the organization yuyv.

To encode an image, use the following command line option:

```
build/bin/<architecture>/jxs_encoder -b <bitrate> -C configs/<configfile>
<input_image> <output_image>
```


where <architecture> is an architecture specific name (e.g. linux64),
<bitrate> is a bitrate in bits per pixel (bpp), and <configfile> is one of the
supplied configuration files. For example, jpeg_xs_main.ini is the
MAIN profile as defined in ISO/IEC 21122-2.

Example:

```
build/bin/linux64/jxs_encoder -b 2 -C configs/jpeg_xs_main.ini
woman.ppm woman.jxs
```

## How to run the decoder ##

The decoder takes a .jxs file and generates from that either a ppm
file (for 444 subsampling) or a yuv file (for 422 subsampling). To
run it, use

`build/bin/<architecture>/jxs_decoder <codestream> <output>`

e.g.

`build/bin/linux64/jxs_decoder woman.jxs out.ppm`


## Encoder and decoder in a single binary ##

Additionally, there is a command line that includes both the encoder
and decoder in a single binary. It reads ppm files and generates ppm
files, or reads yuv files and generates yuv files.

```
build/bin/<architecture>/jxs_enc_dec -b <bitrate> -C configs/<configfile>
<input_image> <output_image>
```

e.g.

```
build/bin/linux64/jxs_enc_dec -b 2 -C configs/jpeg_xs_main.ini
woman.ppm out.ppm
```



