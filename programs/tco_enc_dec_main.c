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


#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <tco.h>
#include "image_open.h"
#include "config.h"
#include "cmdline_options.h"
#include "file_sequence.h"
#include "file_io.h"

int main(int argc, char **argv)
{
	int ret=-1;
	image_t image={0}, *image2 = NULL;
	char *input_seq_n=NULL, *output_seq_n=NULL;
	char* input_fn=NULL, *output_fn=NULL;
	tco_conf_t conf;
	void *bitstream_buf = NULL;
	int bitstream_buf_max_size;
	int bitstream_size = 0;
	tco_enc_context_t *ctx = NULL;
	tco_dec_context_t *ctx_dec = NULL;
	cmdline_options_t options;
	int optind;
	int file_idx = 0;


	fprintf(stderr, "Jpeg XS test model (XSM) version %s\n", tco_get_version_str());


	if ((optind = cmdline_options_parse(argc, argv, CMDLINE_OPT_ENC_DEC, &options)) < 0)
		goto err;

	if (argc - optind != 2) {
		fprintf(stderr, "\nUsage: %s [options] image_in.dpx %s\n", argv[0], "output.dpx");
		fprintf(stderr, "\nFor sequences: %s [options] sequence_in_%%06d.dpx %s\n\n", argv[0], "output_%06d.dpx");
		fprintf(stderr, "options: \n");
		cmdline_options_print_usage(CMDLINE_OPT_ENC_DEC);
		goto err;
	}
	input_seq_n  = argv[optind];
	output_seq_n = argv[optind + 1];


	input_fn = malloc(strlen(input_seq_n) + MAX_SEQ_NUMBER_DIGITS);
	output_fn = malloc(strlen(output_seq_n) + MAX_SEQ_NUMBER_DIGITS);


	sequence_get_filepath( input_seq_n, input_fn, file_idx + options.sequence_first);
	if ((ret=image_open_auto(input_fn, &image, options.width, options.height, options.depth)) < 0)
	{
		fprintf(stderr, "Unable to open image %s!\n", input_fn);
		goto err;
	}

	if (cmdline_options_to_tco_conf(&options, &image, &conf) < 0)
		goto err;

	if ((ret=tco_conf_validate(&conf, &image, NULL)) < 0)
		goto err;

	image2 = tco_clone_image(&image, 0);
	if (!image2)
		goto err;

	tco_free_image(&image);


	bitstream_buf_max_size = (conf.bitstream_nbytes + 7) / 8 * 8;
	bitstream_buf = malloc(bitstream_buf_max_size);

	if (!bitstream_buf) {
		fprintf(stderr, "Unable to allocate codestream mem\n");
		ret = -1;
		goto err;
	}

	ctx = tco_enc_init(&conf, &image, NULL);
	if (!ctx)
	{
		fprintf(stderr, "Unable to allocate encoding context\n");
		goto err;
	}

	ctx_dec = tco_dec_init(&conf, image2);
	if (!ctx_dec)
	{
		fprintf(stderr, "Unable to allocate encoding context\n");
		goto err;
	}
	tco_dec_set_fragment_csv_cbr_path(ctx_dec, options.csv_buffer_model_cbr);
	tco_dec_set_fragment_csv_vbr_path(ctx_dec, options.csv_buffer_model_vbr);

	do
	{
		sequence_get_filepath( output_seq_n, output_fn, file_idx + options.sequence_first);
		if (!fileio_writable(output_fn))
		{
			fprintf(stderr, "Output file is not writable %s\n", output_fn);
			goto err;
		}
		if ((ret=image_open_auto(input_fn, &image, options.width, options.height, options.depth)) < 0)
		{
			fprintf(stderr, "Unable to open image %s!\n", input_fn);
			goto err;
		}
		if (tco_enc_image(ctx, &image, (uint8_t*) bitstream_buf, bitstream_buf_max_size, &bitstream_size) < 0)
		{
			fprintf(stderr, "Unable to encode image %s\n", input_fn);
			goto err;
		}
		tco_free_image(&image);
		fprintf(stderr, "Encoder produced codestream of %lu bytes\n", (unsigned long)bitstream_size);

		if (tco_dec_bitstream(ctx_dec, bitstream_buf, bitstream_size, image2) < 0)
		{
			fprintf(stderr, "Unable to decode encoded image %s\n", input_fn);
			goto err;
		}

		fprintf(stderr, "Codestream decoded!\n");

		if (is_sequence_path( input_seq_n ) ||
			  is_sequence_path( output_seq_n))
			fprintf(stderr, "writing %s\n", output_fn);

		if ((ret=image_save_auto(output_fn, image2)) < 0)
			goto err;

		if (!(is_sequence_path( input_seq_n ) ||
			  is_sequence_path( output_seq_n)))
			break;

		file_idx++;
		if ((options.sequence_n > 0) && (file_idx >= options.sequence_n))
			break;
		sequence_get_filepath( input_seq_n, input_fn, file_idx + options.sequence_first);
	}
	while (fileio_exists(input_fn));

	ret = 0;
err:
	if (bitstream_buf)
		free(bitstream_buf);
	if (image2)
	{
		tco_free_image(image2);
		free(image2);
	}
	tco_free_image(&image);
	if (ctx)
		tco_enc_close(ctx);
	if (ctx_dec)
		tco_dec_close(ctx_dec);
	free(input_fn);
	free(output_fn);
	return ret;
}

