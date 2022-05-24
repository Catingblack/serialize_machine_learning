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
#include <string.h>
#include <assert.h>

#include "../inc/tco.h"
#include "../configs/jpeg_xs_main.h"
#include "../configs/jpeg_xs_high.h"
#include "../configs/jpeg_xs_light_subline.h"
#include "../configs/jpeg_xs_light.h"

#define N_PROFILES (sizeof(profile_list)/sizeof(profile_sel_t))

typedef struct profile_sel_t
{
	int tco_profile;
	tco_conf_t* default_conf;
} profile_sel_t;

profile_sel_t profile_list[] = {
	{ TCO_PROFILE_0      , 			&jpeg_xs_main_conf  	},
	{ TCO_PROFILE_JPEGXS_MAIN, 		&jpeg_xs_main_conf  	},
	{ TCO_PROFILE_JPEGXS_HIGH, 		&jpeg_xs_high_conf  	},
	{ TCO_PROFILE_JPEGXS_LIGHT_SUBLINE, 	&jpeg_xs_light_subline_conf	},
	{ TCO_PROFILE_JPEGXS_LIGHT, &jpeg_xs_light_conf	},
};

int tco_get_default_conf(tco_conf_t* conf, int tco_profile)
{
	int i;
	tco_conf_t* tco_default_conf = NULL;
	for (i = 0; i<N_PROFILES; i++)
	{
		profile_sel_t* l = &profile_list[i];
		if (tco_profile==l->tco_profile)
		{
			tco_default_conf = l->default_conf;
			tco_default_conf->profile = tco_profile;
			break;
		}
	}
	assert(tco_default_conf != NULL);
	*conf = *tco_default_conf;
	return 0;
}
