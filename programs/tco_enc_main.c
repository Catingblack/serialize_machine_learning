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
#include "file_io.h"
#include "file_sequence.h"

 
int main(int argc, char **argv)
{
	int ret=-1;
	image_t image;
	tco_conf_t conf;
	const char* budget_dump_filename = NULL;

	void* bitstream_buf = NULL;
	int bitstream_buf_max_size;
	FILE *file_out = NULL;
	int bitstream_size = 0;
	tco_enc_context_t *ctx = NULL;
	char *input_seq_n = NULL, *input_fn = NULL;
	char *output_seq_n = NULL, *output_fn = NULL;
	cmdline_options_t options;
	int file_idx = 0;
	int optind;

	 
	fprintf(stderr, "Jpeg XS test model (XSM) version %s\n", tco_get_version_str());

	 
	if ((optind = cmdline_options_parse(argc, argv, CMDLINE_OPT_ENCODER,&options)) < 0)
		goto err;

	if (argc - optind != 2) {
		fprintf(stderr, "\nUsage: %s [options] image_in.dpx %s\n", argv[0], "output.tco");
		fprintf(stderr, "\nFor sequences: %s [options] sequence_in_%%06d.dpx %s\n\n", argv[0], "output_%06d.tco");
		fprintf(stderr, "options: \n");
		cmdline_options_print_usage(CMDLINE_OPT_ENCODER);
		goto err;
	}
	input_seq_n = argv[optind];
	output_seq_n = argv[optind + 1];

	 
	input_fn = malloc(strlen(input_seq_n) + MAX_SEQ_NUMBER_DIGITS);
	output_fn = malloc(strlen(output_seq_n) + MAX_SEQ_NUMBER_DIGITS);

	sequence_get_filepath( input_seq_n, input_fn, file_idx + options.sequence_first);
	if (image_open_auto(input_fn, &image, options.width, options.height, options.depth) < 0)
	{
		fprintf(stderr, "Unable to open image %s!\n", input_fn);
		goto err;
	}
	tco_free_image(&image);

	if (cmdline_options_to_tco_conf(&options, &image, &conf) < 0)
		goto err;

	if (tco_conf_validate(&conf, &image, NULL) < 0)
		goto err;

	 
	bitstream_buf_max_size = (conf.bitstream_nbytes + 7) & (~0x7);  
	bitstream_buf = malloc(bitstream_buf_max_size);

	if (!bitstream_buf) {
		fprintf(stderr, "Unable to allocate codestream mem\n");
		goto err;
	}

	 
	ctx = tco_enc_init(&conf, &image, budget_dump_filename);
	if (!ctx)
	{
		fprintf(stderr, "Unable to allocate encoding context\n");
		goto err;
	}

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
		if (is_sequence_path( input_seq_n ) || 
			  is_sequence_path( output_seq_n))
			fprintf(stderr, "writing %s\n", output_fn);
		if (fileio_write(output_fn, bitstream_buf, bitstream_size) < 0)
		{
			fprintf(stderr, "Unable to write output encoded codestream to %s\n", output_fn);
			goto err;
		}
		
		if (!(is_sequence_path( input_seq_n ) || 
			  is_sequence_path( output_seq_n)))
			break;

		file_idx++;
		if ((options.sequence_n > 0) && (file_idx >= options.sequence_n))
			break;
		sequence_get_filepath( input_seq_n, input_fn, file_idx + options.sequence_first);
	}
	while (fileio_exists(input_fn));

	ret=0;  

err:
	if (input_fn)
		free(input_fn);
	if (output_fn)
		free(output_fn);
	if (bitstream_buf)
		free(bitstream_buf);
	if (file_out)
	{
		fclose(file_out);
		if (ret < 0)
			remove(output_fn);
	}
	if (ctx)
		tco_enc_close(ctx);

	tco_free_image(&image);
	return ret;
}
