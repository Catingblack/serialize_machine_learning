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
#include <string.h>

#include <tco.h>
#include "config.h"
#include "image_open.h"
#include "file_io.h"
#include "file_sequence.h"
#include "cmdline_options.h"

int main(int argc, char **argv)
{
	tco_conf_t conf;
	char *input_seq_n=NULL, *output_seq_n=NULL;
	char* input_fn=NULL, *output_fn=NULL;
	image_t image={0};
	uint8_t* bitstream_buf = NULL;
	size_t bitstream_buf_size, bitstream_buf_max_size;
	tco_dec_context_t *ctx=NULL;
	int ret = -1;
	int file_idx = 0;
	cmdline_options_t options;
	int optind;


	fprintf(stderr, "Jpeg XS test model (XSM) version %s\n", tco_get_version_str());

	if ((optind = cmdline_options_parse(argc, argv, CMDLINE_OPT_DECODER,&options)) < 0)
		goto err;

	if (argc - optind != 2)
	{
		fprintf(stderr, "Usage: %s [options] codestream_in.tco image_out.dpx\n", argv[0]);
		fprintf(stderr, "For sequences: [options] %s codestream_%%06d.tco image_out_%%06d.dpx\n", argv[0]);
		fprintf(stderr, "\n");
		cmdline_options_print_usage(CMDLINE_OPT_DECODER);
		goto err;
	}
	input_seq_n  = argv[optind];
	output_seq_n = argv[optind+1];


	input_fn = malloc(strlen(input_seq_n) + MAX_SEQ_NUMBER_DIGITS);
	output_fn = malloc(strlen(output_seq_n) + MAX_SEQ_NUMBER_DIGITS);


	sequence_get_filepath( input_seq_n, input_fn, file_idx + options.sequence_first);
	bitstream_buf_max_size = fileio_getsize(input_fn);
	if (bitstream_buf_max_size <= 0)
	{
		fprintf(stderr, "Unable to open file %s\n", input_seq_n);
		goto err;
	}

	bitstream_buf_max_size = (bitstream_buf_max_size + 8) & (~0x7);

	bitstream_buf = (uint8_t*) malloc(bitstream_buf_max_size);
	if (!bitstream_buf)
	{
		fprintf(stderr, "Unable to allocate memory for codestream\n");
		goto err;
	}
	fileio_read(input_fn, bitstream_buf, &bitstream_buf_size, bitstream_buf_max_size);

	if (tco_dec_probe(bitstream_buf, bitstream_buf_size, &conf, &image))
	{
		fprintf(stderr, "Unable to parse input codestream (%s)\n", input_fn);
		goto err;
	}
	tco_allocate_image(&image);

	ctx = tco_dec_init(&conf, &image);
	if (ctx)
	{
		tco_dec_set_fragment_csv_cbr_path(ctx, options.csv_buffer_model_cbr);
		tco_dec_set_fragment_csv_vbr_path(ctx, options.csv_buffer_model_vbr);
	}
	do
	{
		if (!ctx || (tco_dec_bitstream(ctx, bitstream_buf, bitstream_buf_max_size, &image) < 0))
			goto err;

		sequence_get_filepath( output_seq_n, output_fn, file_idx + options.sequence_first);

		if (is_sequence_path( input_seq_n ) ||
			  is_sequence_path( output_seq_n))
			fprintf(stderr, "writing %s\n", output_fn);


		if (image_save_auto(output_fn, &image) < 0)
			goto err;

		if (!(is_sequence_path( input_seq_n ) ||
			  is_sequence_path( output_seq_n)))
			break;


		file_idx++;
		sequence_get_filepath( input_seq_n, input_fn, file_idx + options.sequence_first);
	}
	while (fileio_read(input_fn, bitstream_buf, &bitstream_buf_size, bitstream_buf_max_size) >= 0);


	ret = 0;
err:
	if (input_fn)
		free(input_fn);
	if (output_fn)
		free(output_fn);
	tco_free_image(&image);
	if (ctx)
		tco_dec_close(ctx);
	if (bitstream_buf)
		free(bitstream_buf);
	return ret;
}
