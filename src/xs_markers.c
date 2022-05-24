/***************************************************************************
** intoPIX SA & Fraunhofer IIS (hereinafter the "Software Copyright       **
** Holder") hold or have the right to license copyright with respect to   **
** the accompanying software (hereinafter the "Software").                **
**                                                                        **
** Copyright License for Evaluation and Testing                           **
** --------------------------------------------                           **
**                                                                        **
** The Software Copyright Holder hereby grants, to any implementer of     **
** this ISO Standard, an irrevocable, non-exclusive, worldwide,           **
** royalty-free, sub-licensable copyright licence to prepare derivative   **
** works of (including translations, adaptations, alterations), the       **
** Software and reproduce, display, distribute and execute the Software   **
** and derivative works thereof, for the following limited purposes: (i)  **
** to evaluate the Software and any derivative works thereof for          **
** inclusion in its implementation of this ISO Standard, and (ii)         **
** to determine whether its implementation conforms with this ISO         **
** Standard.                                                              **
**                                                                        **
** The Software Copyright Holder represents and warrants that, to the     **
** best of its knowledge, it has the necessary copyright rights to        **
** license the Software pursuant to the terms and conditions set forth in **
** this option.                                                           **
**                                                                        **
** No patent licence is granted, nor is a patent licensing commitment     **
** made, by implication, estoppel or otherwise.                           **
**                                                                        **
** Disclaimer: Other than as expressly provided herein, (1) the Software  **
** is provided “AS IS” WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING  **
** BUT NOT LIMITED TO, THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A   **
** PARTICULAR PURPOSE AND NON-INFRINGMENT OF INTELLECTUAL PROPERTY RIGHTS **
** and (2) neither the Software Copyright Holder (or its affiliates) nor  **
** the ISO shall be held liable in any event for any damages whatsoever   **
** (including, without limitation, damages for loss of profits, business  **
** interruption, loss of information, or any other pecuniary loss)        **
** arising out of or related to the use of or inability to use the        **
** Software.”                                                             **
**                                                                        **
** RAND Copyright Licensing Commitment                                    **
** -----------------------------------                                    **
**                                                                        **
** IN THE EVENT YOU WISH TO INCLUDE THE SOFTWARE IN A CONFORMING          **
** IMPLEMENTATION OF THIS ISO STANDARD, PLEASE BE FURTHER ADVISED THAT:   **
**                                                                        **
** The Software Copyright Holder agrees to grant a copyright              **
** license on reasonable and non- discriminatory terms and conditions for **
** the purpose of including the Software in a conforming implementation   **
** of the ISO Standard. Negotiations with regard to the license are       **
** left to the parties concerned and are performed outside the ISO.       **
**                                                                        **
** No patent licence is granted, nor is a patent licensing commitment     **
** made, by implication, estoppel or otherwise.                           **
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lvls_map.h"
#include "version.h"
#include "tco_conf.h"
#include "bitpacking.h"
#include "xs_markers.h"



int xs_write_head(bit_packer_t* bitstream, image_t* im, tco_conf_t *conf)
{
	int nbits = 0;

	nbits += bitpacker_write(bitstream, XS_MARKER_SOC, XS_MARKER_NBITS);
	nbits += xs_write_capabilities_marker(bitstream, im, conf);
	nbits += xs_write_picture_header(bitstream, im, conf);
	nbits += xs_write_component_table(bitstream, im, conf);
	nbits += xs_write_weights_table(bitstream, im, conf);
	nbits += xs_write_extension_marker(bitstream, im, conf);
	return nbits;
}


int xs_parse_head(bit_unpacker_t* bitstream, image_t* im, tco_conf_t *conf)
{
	int nbits = 0;
	uint64_t val;
	int sz;

	tco_get_default_conf(conf, TCO_PROFILE_JPEGXS_MAIN);

	nbits += bitunpacker_read(bitstream, &val, XS_MARKER_NBITS);
	assert(val == XS_MARKER_SOC);

	do {
		sz = bitunpacker_peek(bitstream,&val,16);
		assert(sz == 16);

		switch(val) {
		case XS_MARKER_CAP:
			nbits += xs_parse_capabilities_marker(bitstream, im, conf);
			break;
		case XS_MARKER_PIH:
			nbits += xs_parse_picture_header(bitstream, im, conf);
			break;
		case XS_MARKER_CDT:
			nbits += xs_parse_component_table(bitstream, im, conf);
			break;
		case XS_MARKER_WGT:
			nbits += xs_parse_weights_table(bitstream, im, conf);
			break;
		case XS_MARKER_COM:
			nbits += xs_parse_extension_marker(bitstream, im, conf);
			break;
		case XS_MARKER_SLH:
			return nbits;
		default:
			assert(!"unexpected marker found");
			return -1;
		}
	} while(1);

	return -1;
}

int xs_write_tail(bit_packer_t* bitstream)
{
	return bitpacker_write(bitstream, XS_MARKER_EOC, XS_MARKER_NBITS);
}

int xs_parse_tail(bit_unpacker_t* bitstream)
{
	int nbits = 0;
	uint64_t val;
	nbits += bitunpacker_read(bitstream, &val, XS_MARKER_NBITS);
	assert(val==XS_MARKER_EOC);
	return nbits;
}

int xs_write_capabilities_marker(bit_packer_t* bitstream, image_t* im, tco_conf_t *conf)
{
	int nbits = 0;
	int lcap;
	nbits += bitpacker_write(bitstream, XS_MARKER_CAP, XS_MARKER_NBITS);
	lcap = 2;
	nbits += bitpacker_write(bitstream, lcap, XS_MARKER_NBITS);
	return nbits;
}

int xs_parse_capabilities_marker(bit_unpacker_t* bitstream, image_t* im, tco_conf_t *conf)
{
	int nbits = 0;
	uint64_t val;

	nbits += bitunpacker_read(bitstream, &val, XS_MARKER_NBITS);
	assert(val==XS_MARKER_CAP);

	nbits += bitunpacker_read(bitstream, &val, XS_MARKER_NBITS);
	conf->profile = TCO_PROFILE_JPEGXS_MAIN;
	return nbits;
}


int xs_write_picture_header(bit_packer_t* bitstream, image_t* im, tco_conf_t *conf)
{
	int nbits=0;
	int lpih=26;
	int precinct_height=(1<<(conf->ndecomp_v));

	nbits += bitpacker_write(bitstream, XS_MARKER_PIH, XS_MARKER_NBITS);
	nbits += bitpacker_write(bitstream, lpih, XS_MARKER_NBITS);
	nbits += bitpacker_write(bitstream, conf->bitstream_nbytes, 32);
	nbits += bitpacker_write(bitstream,   0, 16);
	nbits += bitpacker_write(bitstream,   0, 16);
	nbits += bitpacker_write(bitstream, im->w[0], 16);
	nbits += bitpacker_write(bitstream, im->h[0], 16);

	if (conf->col_sz != im->w[0])
	{
		assert((conf->col_sz % conf->col_granularity)== 0);
		nbits += bitpacker_write(bitstream, conf->col_sz / conf->col_granularity, 16);
	}
	else
	{
		nbits += bitpacker_write(bitstream, 0, 16);
	}
	nbits += bitpacker_write(bitstream, (conf->slice_height)/(precinct_height), 16);
	nbits += bitpacker_write(bitstream, im->ncomps, 8);
	nbits += bitpacker_write(bitstream, conf->group_size, 8);
	nbits += bitpacker_write(bitstream, conf->sigflags_group_width, 8);
	nbits += bitpacker_write(bitstream, conf->in_depth, 8);
	nbits += bitpacker_write(bitstream, conf->quant, 4);
	nbits += bitpacker_write(bitstream, 4, 4);
	nbits += bitpacker_write(bitstream, 0, 1);
	nbits += bitpacker_write(bitstream, 0, 3);
	nbits += bitpacker_write(bitstream, (conf->rct && !im->is_422), 4);
	nbits += bitpacker_write(bitstream, conf->ndecomp_h, 4);
	nbits += bitpacker_write(bitstream, conf->ndecomp_v, 4);
	nbits += bitpacker_write(bitstream, conf->dq_type, 4);
	nbits += bitpacker_write(bitstream, conf->sign_opt & 0x1, 2);
	nbits += bitpacker_write(bitstream, conf->gc_run_sigflags_zrcsf, 2);
	return nbits;
}

int xs_parse_picture_header(bit_unpacker_t* bitstream, image_t* im, tco_conf_t *conf)
{
	int nbits=0;
	int lpih=26;
	int precinct_height;
	uint64_t val;


	tco_get_default_conf(conf, TCO_CONF_DEFAULT_PROFILE);

	nbits += bitunpacker_read(bitstream, &val, XS_MARKER_NBITS);
	assert(val==XS_MARKER_PIH);
	nbits += bitunpacker_read(bitstream, &val, XS_MARKER_NBITS);
	assert(val==lpih);
	nbits += bitunpacker_read(bitstream, &val, 32);
	conf->bitstream_nbytes = val;

	nbits += bitunpacker_read(bitstream, &val, 16);

	nbits += bitunpacker_read(bitstream, &val, 16);

	nbits += bitunpacker_read(bitstream, &val, 16);
	im->w[0] = val;

	nbits += bitunpacker_read(bitstream, &val, 16);
	im->h[0] = val;


	nbits += bitunpacker_read(bitstream, &val, 16);
	conf->col_sz = val;

	nbits += bitunpacker_read(bitstream, &val, 16);
	conf->slice_height = val;
	nbits += bitunpacker_read(bitstream, &val, 8);
	im->ncomps = val;
	nbits += bitunpacker_read(bitstream, &val, 8);
	conf->group_size = val;
	nbits += bitunpacker_read(bitstream, &val, 8);
	conf->sigflags_group_width = val;
	nbits += bitunpacker_read(bitstream, &val, 8);
	conf->in_depth = val;
	nbits += bitunpacker_read(bitstream, &val, 4);
	conf->quant = val;
	nbits += bitunpacker_read(bitstream, &val, 4);
	assert(val==4);
	nbits += bitunpacker_read(bitstream, &val, 1);
	assert(val==0);
	nbits += bitunpacker_read(bitstream, &val, 3);
	assert(val==0);
	nbits += bitunpacker_read(bitstream, &val, 4);
	conf->rct = val;
	nbits += bitunpacker_read(bitstream, &val, 4);
	conf->ndecomp_h = val;
	nbits += bitunpacker_read(bitstream, &val, 4);
	conf->ndecomp_v = val;
	precinct_height = (1 << conf->ndecomp_v);
	conf->slice_height *= precinct_height;
	nbits += bitunpacker_read(bitstream, &val, 4);
	conf->dq_type = val;
	nbits += bitunpacker_read(bitstream, &val, 2);
	conf->sign_opt = val;
	nbits += bitunpacker_read(bitstream, &val, 2);
	conf->gc_run_sigflags_zrcsf = val;
	conf->gc_run_sigflags_zrf = (!val);

	/*
	** thor fix: the column size in pixels is not known until
	** the component table is read
	conf->col_sz *= (8*(1<<conf->ndecomp_h));
	**
	*/
	conf->col_max_width = 0;

	return nbits;
}

int xs_write_component_table(bit_packer_t* bitstream, image_t* im, tco_conf_t *conf)
{
	int nbits=0;
	int lcdt= 2 * im->ncomps + 2;
	int comp;

	nbits += bitpacker_write(bitstream, XS_MARKER_CDT, XS_MARKER_NBITS);
	nbits += bitpacker_write(bitstream, lcdt, XS_MARKER_NBITS);

	for (comp = 0; comp < im->ncomps; comp++)
	{
		nbits += bitpacker_write(bitstream, im->depth, 8);

		if (im->w[comp] != im->w[0])
			nbits += bitpacker_write(bitstream, 2, 4);
		else
			nbits += bitpacker_write(bitstream, 1, 4);


		nbits += bitpacker_write(bitstream, 1, 4);
	}
	return nbits;
}

int xs_parse_component_table(bit_unpacker_t* bitstream, image_t* im, tco_conf_t *conf)
{
	int nbits=0;
	uint64_t val;
	int comp;
	int maxsub = 1; /* maximum horizontal component subsampling for column size computation */
	int lcdt= 2 * im->ncomps + 2;
	int lcdt_tor = 3 * im->ncomps + 2;

	int bits_for_subsampling = 4;

	nbits += bitunpacker_read(bitstream, &val, XS_MARKER_NBITS);
	assert(val==XS_MARKER_CDT);
	nbits += bitunpacker_read(bitstream, &val, XS_MARKER_NBITS);
	if ((val != lcdt) && (val == lcdt_tor))
	{
		fprintf(stderr, "Warning : Wrong length for LCDT - continuing assuming 8 bits for subsampling values\n");
		bits_for_subsampling = 8;
	}
	else
		assert(val==lcdt);

	for (comp = 0; comp < im->ncomps; comp++)
	{
		nbits += bitunpacker_read(bitstream, &val, 8);
		im->depth = val;


		nbits += bitunpacker_read(bitstream, &val, bits_for_subsampling);
		if (val==1)
			im->w[comp] = im->w[0];
		else if (val==2)
			im->w[comp] = im->w[0]/2;
		else
			assert(!"unable to parse component table (horizontal subsampling)\n");

		if (val > maxsub)
			maxsub = val;

		nbits += bitunpacker_read(bitstream, &val, bits_for_subsampling);
		if (val==1)
			im->h[comp] = im->h[0];
		else if (val==2)
			im->h[comp] = im->h[0]/2;
		else
			assert(!"unable to parse component table (vertical subsampling)\n");
	}
	if ((im->ncomps == 3) && (im->w[0]/2 == im->w[1]) && (im->w[0]/2 == im->w[2]))
		im->is_422 = 1;
	else
		im->is_422 = 0;

	/* thor fix: fixup the column size to samples */
	conf->col_sz *= (8*maxsub*(1<<conf->ndecomp_h));

	return nbits;
}

int xs_write_weights_table(bit_packer_t* bitstream, image_t* im, tco_conf_t *conf)
{
	int nbits = 0;
	int lvl, comp;
	int Nl = map_get_NL(im->ncomps, conf->ndecomp_h, conf->ndecomp_v);
	int Nb = map_get_N_filtertypes(conf->ndecomp_h, conf->ndecomp_v);
	int idx = 0;

	nbits += bitpacker_write(bitstream, XS_MARKER_WGT, XS_MARKER_NBITS);
	nbits += bitpacker_write(bitstream, 2*Nl + 2, XS_MARKER_NBITS);
	for (lvl = 0; lvl < Nb; lvl++)
	{
		for (comp = 0; comp < im->ncomps; comp++)
		{
			idx = Nb*comp + lvl;
			nbits += bitpacker_write(bitstream, conf->lvl_gains[idx], 8);
			nbits += bitpacker_write(bitstream, conf->lvl_priorities[idx], 8);

		}
	}
	return nbits;
}

int xs_parse_weights_table(bit_unpacker_t* bitstream, image_t* im, tco_conf_t *conf)
{
	int nbits = 0;
	int lvl, comp;
	int Nl = map_get_NL(im->ncomps, conf->ndecomp_h, conf->ndecomp_v);
	int Nb = map_get_N_filtertypes(conf->ndecomp_h, conf->ndecomp_v);
	int idx = 0;
	uint64_t val;

	conf->gains_choice = GAINS_CHOICE_MANUAL;

	nbits += bitunpacker_read(bitstream, &val, XS_MARKER_NBITS);
	assert(val == XS_MARKER_WGT);

	nbits += bitunpacker_read(bitstream, &val, XS_MARKER_NBITS);
	assert(val == 2*Nl + 2);

	for (lvl = 0; lvl < Nb; lvl++)
	{
		for (comp = 0; comp < im->ncomps; comp++)
		{
			idx = Nb*comp + lvl;
			nbits += bitunpacker_read(bitstream, &val, 8);
			conf->lvl_gains[idx] = val;
			nbits += bitunpacker_read(bitstream, &val, 8);
			conf->lvl_priorities[idx] = val;
		}
	}
	return nbits;
}

int xs_write_extension_marker(bit_packer_t* bitstream, image_t* im, tco_conf_t *conf)
{
	return 0;
}

int xs_parse_extension_marker(bit_unpacker_t* bitstream, image_t* im, tco_conf_t *conf)
{
	int nbits = 0;
	uint64_t size,val;

	nbits += bitunpacker_read(bitstream, &val, XS_MARKER_NBITS);
	assert(val==XS_MARKER_COM);
	nbits += bitunpacker_read(bitstream, &size, XS_MARKER_NBITS);
	assert(size >= 4);

	while(size > 2)
	{
		nbits += bitunpacker_read(bitstream, &val, 8);
		size--;
	}

	return nbits;
}

int xs_write_slice_header(bit_packer_t* bitstream, int slice_idx)
{
	int nbits = 0;
	nbits += bitpacker_write(bitstream, XS_MARKER_SLH, XS_MARKER_NBITS);
	nbits += bitpacker_write(bitstream, 4, XS_MARKER_NBITS);
	nbits += bitpacker_write(bitstream, slice_idx, 16);
	assert(nbits == XS_SLICE_HEADER_NBYTES*8);
	return nbits;
}

int xs_parse_slice_header(bit_unpacker_t* bitstream, int* slice_idx)
{
	int nbits = 0;
	uint64_t val;
	nbits += bitunpacker_read(bitstream, &val, XS_MARKER_NBITS);
	assert(val == XS_MARKER_SLH);
	nbits += bitunpacker_read(bitstream, &val, XS_MARKER_NBITS);
	assert(val == 4);
	nbits += bitunpacker_read(bitstream, &val, 16);
	*slice_idx = (int) val;
	return nbits;
}


#ifdef TEST_XS_MARKERS

int main()
{
	uint8_t storage[2048];
	image_t im, im2;
	tco_conf_t conf, conf2;
	bit_packer_t* bitstream=bitpacker_open(storage, 2048);
	bit_unpacker_t* bitstream2;
	int len;
	int i;
	int len_check;

	im.ncomps = 3;
	im.w[0] = 1920;
	im.w[1] = 960;
	im.w[2] = 960;
	im.h[0] = 1080;
	im.h[1] = 1080;
	im.h[2] = 1080;

	conf.profile = TCO_PROFILE_JPEGXS;
	conf.bitstream_nbytes = 1234567;
	conf.slice_height = 64;
	conf.group_size = 4;
	conf.sigflags_group_width = 8;
	conf.in_depth = 20;
	conf.quant = 4;
	conf.rct = 0;
	conf.ndecomp_h = 5;
	conf.ndecomp_v = 1;
	conf.dq_type = 0;
	conf.sign_opt = 1;
	conf.gc_run_sigflags_zrcsf = 1;

	len_check = xs_write_head(bitstream, &im, &conf);
	len = bitpacker_get_len(bitstream);
	assert(len%8==0);
	assert(len==len_check);
	fprintf(stderr, "Head Length (bytes) %d\n", len/8);

	xs_write_tail(bitstream);
	len = bitpacker_get_len(bitstream) - len;
	assert(len%8==0);
	fprintf(stderr, "Tail Length (bytes) %d\n", len/8);

	len = bitpacker_get_len(bitstream) / 8;
	bitpacker_close(bitstream);

	for (i=0; i<len; i++)
	{
		fprintf(stderr, "%02X%s", storage[i], (i%16==15) ? "\n" : "");
	}
	fprintf(stderr, "\n");

	bitstream2=bitunpacker_open(storage, 1024);
	len_check = xs_parse_head(bitstream2, &im2, &conf2);
	len = bitunpacker_consumed_bits(bitstream2);
	fprintf(stderr, "head read length (bytes) %d\n", len/8);
	assert(len_check == len);

	assert(im2.ncomps == im.ncomps);
	assert(im2.w[0] == im.w[0]);
	assert(im2.w[1] == im.w[1]);
	assert(im2.w[2] == im.w[2]);
	assert(im2.h[0] == im.h[0]);
	assert(im2.h[1] == im.h[1]);
	assert(im2.h[2] == im.h[2]);
	assert(conf2.profile == conf.profile);
	assert(conf2.bitstream_nbytes == conf.bitstream_nbytes);
	assert(conf2.slice_height == conf.slice_height);
	assert(conf2.group_size == conf.group_size);
	assert(conf2.sigflags_group_width == conf.sigflags_group_width);
	assert(conf2.in_depth == conf.in_depth);
	assert(conf2.quant == conf.quant);
	assert(conf2.rct == conf.rct);
	assert(conf2.ndecomp_h == conf.ndecomp_h);
	assert(conf2.ndecomp_v == conf.ndecomp_v);
	assert(conf2.dq_type == conf.dq_type);
	assert(conf2.sign_opt == conf.sign_opt);
	assert(conf2.gc_run_sigflags_zrcsf == conf.gc_run_sigflags_zrcsf);

	return 0;
}

#endif
