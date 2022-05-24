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
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <tco.h>

#include "cmdline_options.h"
#include "config.h"

#ifndef DISABLE_PROFILE0
#define CONF_FILE_SWITCH "C:"
#else
#define CONF_FILE_SWITCH ""
#endif

char* active_options[]={
	  CONF_FILE_SWITCH "p:b:s:o:w:h:d:f:n:I:v",
	  "F:V:n:f:v",
	  CONF_FILE_SWITCH "F:V:p:b:s:o:w:h:d:f:n:I:v",
};

struct {
	char letter;
	char* usage_msg;
}
options_help[]=
{
#ifndef DISABLE_PROFILE0
	{'p',"-p profile : specify JPEG XS profile (0 for dev, 3 for XS_main, 4 for XS_high, 5 for XS_light_subline, 6 for XS_light)\n"},
#else
	{'p',"-p profile : specify JPEG XS profile (3 for XS)\n"},
#endif
	{'C' , "-C config.ini : configuration file (bitstream will be marked as profile0 when conf file is used)\n"},
	{'b' , "-b bpp : target bit per pixel (including file header size\n"},
	{'s' , "-s size : target codestream size (bytes) (including file header size)\n"},
	{'s' , "\t Use size OR bpp, not both.\n"},
	{'w' , "-w width -h height: image dimension (required for raw input format like v210, yuv16, rgb16)\n"},
	{'d' , "-d depth image depth (required for rgb16/yuv16 input format)\n"},
	{'I' , "-I encoder_id: hexadecimal value (example 0x12345678) to put in the Encoder ID field of tico header\n"},
	{'o' , "-o psnr/visual : allocate rate to have the max PSNR (psnr) or to have the errors less visible (visual)\n"},
	{'f' , "-f nb: index of first image of sequence (N/A for single file operation)\n"},
	{'n' , "-n nb: number of images from sequence to process (N/A for single file operation)\n"},
	{'F' , "-F filename: buffer model : generate CSV file with buffer model information (cbr)\n"},
	{'V' , "-V filename: buffer model : generate CSV file with buffer model information (vbr)\n"},
#ifndef DISABLE_PROFILE0
	{'v' , "-v: print debug information about compression\n"},
#endif
	{0, NULL},
};



int cmdline_options_parse(int argc, char **argv, int program_idx, cmdline_options_t* options)
{
	int opt;


	options->width = -1;
	options->height = -1;
	options->depth = -1;
	options->cfg_fn = NULL;
	options->bpp = -1;
	options->profile = -1;
	options->optimisation_psnr = 1;
	options->enc_id = 0;
	options->enc_id_set = 0;
	options->sequence_first = 0;
	options->sequence_n = -1;
	options->verbose = 0;
	options->csv_buffer_model_cbr = NULL;
	options->csv_buffer_model_vbr = NULL;

	while ((opt = getopt(argc, argv, active_options[program_idx])) != -1)
	{
		switch(opt)
		{
#ifndef DISABLE_PROFILE0
			case 'C':
				options->cfg_fn = optarg;
				break;
#endif
			case 'b':
				options->bpp = atof(optarg);
				break;
			case 's':
				options->size = atoi(optarg);
				break;
			case 'w':
				options->width = atoi(optarg);
				break;
			case 'h':
				options->height = atoi(optarg);
				break;
			case 'd':
				options->depth = atoi(optarg);
				break;
			case 'I':
				options->enc_id_set = 1;
				sscanf(optarg,"%X",&(options->enc_id));
				break;
			case 'o':
			{
				if (strcmp(optarg,"visual") == 0)
				{
					options->optimisation_psnr = 0;
				}
				else if (strcmp(optarg,"psnr") != 0)
				{
					fprintf(stderr, "Error: unsupported value %s for option -o (optimisation type). Please use 'psnr' or 'visual'\n", optarg);
					goto err;
				}
				break;
			}
			case 'p':
			{
#ifndef DISABLE_PROFILE0
				if (atoi(optarg)==0)
				{
					options->profile = TCO_PROFILE_0;
				}
				else
#endif
				if (atoi(optarg)>=3)
				{
					options->profile = TCO_PROFILE_JPEGXS_MAIN + atoi(optarg) - 3;
				}
				else
				{
					fprintf(stderr, "Error: Unknown profile ! %d\n", atoi(optarg));
					goto err;
				}
				break;
			}
			case 'f':
			{
				options->sequence_first = atoi(optarg);
				break;
			}
			case 'n':
			{
				options->sequence_n = atoi(optarg);
				break;
			}
#ifndef DISABLE_PROFILE0
			case 'v':
			{
				options->verbose = 1;
				break;
			}
#endif
			case 'F':
			{
				options->csv_buffer_model_cbr = optarg;
				break;
			}
			case 'V':
			{
				options->csv_buffer_model_vbr = optarg;
				break;
			}
		}
	}
	return optind;
err:
	return -1;
}


int cmdline_options_print_usage(int program_idx)
{
	int i=0;
	for (i=0; i<strlen(active_options[program_idx]); i++)
	{
		int j=0;
		while (options_help[j].letter != 0)
		{
			if (options_help[j].letter == active_options[program_idx][i])
			{
				fprintf(stderr,"%s", options_help[j].usage_msg);
			}
			j++;
		}
	}
	return 0;
}


int cmdline_options_to_tco_conf(cmdline_options_t* options, image_t* image, tco_conf_t* conf)
{



	if (options->cfg_fn != NULL)
	{


		if (options->profile == -1)
			conf->profile = TCO_PROFILE_0;

		if (config_parse(options->cfg_fn, conf) < 0)
			return -1;

		if (options->profile == -1)
			options->profile = conf->profile;
	}



	tco_get_default_conf(conf, options->profile != -1 ? options->profile : TCO_CONF_DEFAULT_PROFILE);

	if (options->cfg_fn != NULL)
	{
		fprintf(stderr, "Note: 'ini' file mode\n");
		fprintf(stderr, "Note: Profile %d default conf are used\n", options->profile);
		fprintf(stderr, "Note: keys from ini file will overwrite those default values\n");
		if (config_parse(options->cfg_fn, conf) < 0)
			return -1;
	}


	if (conf->bpp > 0)
	{
		options->size = BPP2BYTESIZE( conf->bpp, image->h[0], image->w[0]);
		conf->bpp = -1;
	}
	if (options->bpp > 0)
		options->size = BPP2BYTESIZE( options->bpp, image->h[0], image->w[0]);
	if (options->size > 0)
		conf->bitstream_nbytes = options->size;

	if (conf->gains_choice != GAINS_CHOICE_MANUAL)
	{
		if (options->optimisation_psnr)
			conf->gains_choice = GAINS_CHOICE_PSNR;
		else
			conf->gains_choice = GAINS_CHOICE_VISUAL;
	}

	if (options->enc_id_set == 1)
		conf->encoder_id = options->enc_id;

	if (options->profile != -1)
		conf->profile = options->profile;

	conf->verbose = options->verbose;

	return 0;
}

