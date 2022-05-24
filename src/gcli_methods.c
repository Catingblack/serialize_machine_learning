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
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include "gcli_methods.h"
#include "../std/bsr.h"

static char* alphabet_names[] = {
	"RAW",
	"BND",
};
static char* pred_names[] = {
	"_NOPRED",
	"_VER",
	"_---",
};
static char* run_names[] = {
	"_NORUN",
	"_ZRF",
	"_ZRCSF",
	"_---",
};

void print_method(FILE* f, int gcli_method)
{
	fprintf(f, "%s%s%s",
			alphabet_names[method_get_alphabet(gcli_method)],
			(method_get_alphabet(gcli_method) == ALPHABET_RAW_4BITS) ? "" : pred_names[method_get_pred(gcli_method)],
			(method_get_alphabet(gcli_method) == ALPHABET_RAW_4BITS) ? "" : run_names[method_get_run(gcli_method)]);
}

char* method_to_str(char* str, int gcli_method) {
	sprintf(str, "%s%s%s", alphabet_names[method_get_alphabet(gcli_method)],
		(method_get_alphabet(gcli_method) == ALPHABET_RAW_4BITS) ? "" : pred_names[method_get_pred(gcli_method)],
		(method_get_alphabet(gcli_method) == ALPHABET_RAW_4BITS) ? "" : run_names[method_get_run(gcli_method)]);
	return str;
}

uint64_t gcli_method_get_enabled(const tco_conf_t* conf)
{
	uint64_t enabled_alphabets = 0;
	uint64_t enabled_predictions = 0;
	uint64_t enabled_runs = 0;
	uint64_t enabled_modes = 0;

	if (conf->gc_raw) 	      	enabled_alphabets |= (1ULL << ALPHABET_RAW_4BITS);
	if (conf->gc_bounded) 	  	enabled_alphabets |= (1ULL << ALPHABET_UNARY_UNSIGNED_BOUNDED);

	if (conf->gc_ver)			enabled_predictions |= (1ULL << PRED_VER);
	if (conf->gc_nopred)		enabled_predictions |= (1ULL << PRED_NONE);

	if (conf->gc_run_none)	    enabled_runs |= (1ULL << RUN_NONE);
	if (conf->gc_run_sigflags_zrf)	enabled_runs |= (1ULL << RUN_SIGFLAGS_ZRF);
	if (conf->gc_run_sigflags_zrcsf)	enabled_runs |= (1ULL << RUN_SIGFLAGS_ZRCSF);

	enabled_modes = (enabled_alphabets << METHOD_ENABLE_MASK_ALPHABETS_OFFSET) |
					(enabled_predictions << METHOD_ENABLE_MASK_PREDICTIONS_OFFSET) |
					(enabled_runs << METHOD_ENABLE_MASK_RUNS_OFFSET);

	return enabled_modes;
}

uint64_t gcli_method_get_enabled_ver(uint64_t enabled)
{
	return (enabled & (~(1ULL << (PRED_NONE+METHOD_ENABLE_MASK_PREDICTIONS_OFFSET))));
}

uint64_t gcli_method_get_enabled_nover(uint64_t enabled)
{
	return (enabled & (~(1ULL << (PRED_VER+METHOD_ENABLE_MASK_PREDICTIONS_OFFSET))));
}

int gcli_method_is_enabled(uint64_t enabled, int gcli_method, int precinct_group)
{
	#define is_run_enabled(run) ((1ULL << (run)) & (enabled_runs))
	uint64_t enabled_alphabets = (enabled >> METHOD_ENABLE_MASK_ALPHABETS_OFFSET) & ((1ULL << ALPHABET_COUNT) - 1);
	uint64_t enabled_predictions = (enabled >> METHOD_ENABLE_MASK_PREDICTIONS_OFFSET) & ((1ULL << PRED_COUNT) - 1);
	uint64_t enabled_runs = (enabled >> METHOD_ENABLE_MASK_RUNS_OFFSET) & ((1ULL << RUN_COUNT) - 1);

	int	alphabet = method_get_alphabet(gcli_method);
	int	pred = method_get_pred(gcli_method);
	int	run = method_get_run(gcli_method);

	if (!((1ULL << alphabet) & enabled_alphabets))
		return 0;

	if (alphabet != ALPHABET_RAW_4BITS)
	{
		if (!is_run_enabled(run))
		{
			return 0;
		}
		if (!((1ULL << pred) & enabled_predictions))
			return 0;

		if ((precinct_group==PRECINCT_FIRST_OF_SLICE) && (pred == PRED_VER))
			return 0;
	}
	else
	{
		if (run != RUN_NONE)
			return 0;
		if (pred != PRED_NONE)
			return 0;
	}
	return 1;
}

int gcli_method_get_signaling_nbits(uint64_t enabled_methods)
{
	return 2;
}

int gcli_method_get_signaling(int gcli_method, uint64_t enabled_methods)
{
	int uses_run = ((method_get_run(gcli_method) == RUN_SIGFLAGS_ZRF) || (method_get_run(gcli_method) == RUN_SIGFLAGS_ZRCSF));
	if (gcli_method == method_get_idx(ALPHABET_RAW_4BITS,0,0))
		return -1;
	int signaling = (uses_run ? 0x2 : 0) | ((method_get_pred(gcli_method) == PRED_VER) ? 0x1 : 0);
	return signaling;
}

int gcli_method_from_signaling(int signaling, uint64_t enabled_methods)
{
	int gcli_method;
	for (gcli_method = 0; gcli_method < GCLI_METHODS_NB; gcli_method++)
		if ((gcli_method_is_enabled(enabled_methods, gcli_method, PRECINCT_ALL)) &&
			gcli_method_get_signaling(gcli_method, enabled_methods)==signaling)
			return gcli_method;
	assert(!"Bad GCLI Method Signaling\n");
	return -1;
}

void print_enabled_methods(tco_conf_t* pconf)
{
	int gcli_method;
	uint64_t enabled_methods = ((uint64_t)-1);
	enabled_methods = gcli_method_get_enabled(pconf);

	for (gcli_method = 0; gcli_method < GCLI_METHODS_NB; gcli_method++)
	{
		if (gcli_method_is_enabled(enabled_methods, gcli_method, 1))
		{
			fprintf(stderr, "gcli_method=%08X ", gcli_method);
			print_method(stderr, gcli_method);
		}
	}
}


#ifdef TEST_GCLI_METHODS

int main()
{
	tco_conf_t conf = {0}, *pconf=&conf;

	pconf->gc_pred_per_sb = 0;
	pconf->gc_raw = 1;
	pconf->gc_bounded = 1;
	pconf->gc_ver = 1;
	pconf->gc_nopred = 1;
	pconf->gc_run_none = 1;
	pconf->gc_run_sigflags_zrf = 1;
	pconf->gc_run_sigflags_zrcsf = 1;
	pconf->gc_run_per_sb = 1;

	print_enabled_methods(pconf);

	return 0;
}

#endif
