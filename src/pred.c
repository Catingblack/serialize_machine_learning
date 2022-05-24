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


#include "pred.h"
#include "common.h"
#include <stdio.h>

int tco_pred_ver_compute_predictor(int gcli_top, int gtli_top, int gtli, int top_trunc_method)
{
	int top;
	if (top_trunc_method == 0)
	{
		top = MAX(gcli_top, MAX(gtli, gtli_top));
	}
	else
	{
		if ( gcli_top > gtli_top )
		{
			top = MAX(gcli_top, gtli);
		}
		else
		{
			top = MAX(0, gtli);
		}
	}
	return top;
}

int tco_pred_ver(gcli_data_t* gcli_buf, gcli_data_t* gcli_buf_top, int buf_len, gcli_pred_t* pred_buf, 
				gcli_data_t* predictors, int gtli, int gtli_top, int top_trunc_method)
{
	int i;
	if (buf_len > 0)
	{
		for (i=0; i < buf_len; i++)
		{
			predictors[i] = tco_pred_ver_compute_predictor(gcli_buf_top[i], gtli_top, gtli, top_trunc_method);
			pred_buf[i] = MAX(gcli_buf[i],gtli) - predictors[i];
		}
	}
	return 0;
}

int tco_pred_ver_inverse(gcli_pred_t* pred_buf, gcli_data_t* gcli_buf_top, int buf_len, gcli_data_t* gcli_buf, int gtli, int gtli_top, int top_trunc_method)
{
	int i;
	int top;
	if (buf_len > 0)
	{
		for (i=0; i<buf_len; i++)
		{
			top = tco_pred_ver_compute_predictor(gcli_buf_top[i], gtli_top, gtli, top_trunc_method);
			gcli_buf[i] = top + pred_buf[i];
			 
			 
			if (gcli_buf[i] <= gtli)
				gcli_buf[i] -= gtli;
		}
	}
	return 0;
}

int tco_pred_ver_check_gclis(const gcli_data_t* gcli_buf, int buf_len)
{
	int i;
	if (buf_len > 0)
	{
		for (i = 0; i<buf_len; i++)
		{
			return 0;
		}
	}
	return 1;
}

int tco_pred_none(const gcli_data_t* gcli_buf, int buf_len, gcli_pred_t* pred_buf, int gtli)
{
	int i;
	for (i=0; i<buf_len; i++)
		pred_buf[i] = MAX(0, gcli_buf[i] - gtli);
	return 0;
}

int tco_pred_none_inverse(gcli_pred_t* pred_buf, int buf_len, gcli_data_t* gcli_buf, int gtli)
{
	int i;
	for (i=0; i<buf_len; i++)
		gcli_buf[i] = (pred_buf[i] > 0) ? pred_buf[i] + gtli : 0;
	return 0;
}

#ifdef TEST_PRED

#define DUMP_BUF(buf, len) for (i=0; i<(len); i++) \
						      fprintf(stderr,"%d ", (buf)[i]); \
						      fprintf(stderr,"\n");

int check_result(gcli_data_t* gcli_buf, gcli_data_t* gcli_buf_out, gcli_pred_t* pred_buf,int len, int gtli)
{
	int i;
	for (i=0; i<len; i++)
	{
		if (((gcli_buf_out[i] != gcli_buf[i]) && (gcli_buf[i] > gtli)) ||
			((gcli_buf_out[i] != 0) && (gcli_buf[i] <= gtli)))
		{
			return -1;
		}
	}
	return 0;
}

int main(int argc, char* argv[])
{
	#define NSAMPLES (sizeof(gcli_buf)/sizeof(gcli_data_t))
	gcli_data_t gcli_buf[] = {
		8,8,4,7,7,8,7,7,7,7,
		8,7,8,6,7,7,8,6,7,7,
		0,4,0,0,5,0,0,6,0,0,
		5,0,0,6,0,0,5,5,0,0,
		6,0,0,5,0,0,6,0,0,5,
		0,0,6,0,0,5,0,7,7,7,
		7,7,6,7,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,3,2,5,1,7,8,
	};
	gcli_data_t gcli_buf_top[] = {
		7,7,5,7,7,6,7,7,6,6,
		7,9,4,2,3,2,7,6,7,7,
		0,4,0,0,5,0,0,5,0,0,
		5,0,0,6,0,0,4,4,0,0,
		6,0,0,5,0,0,6,0,0,5,
		0,0,6,0,0,5,0,7,7,7,
		7,7,6,7,1,1,1,0,0,0,
		0,0,0,0,0,0,1,0,0,0,
		0,0,0,0,0,0,1,2,1,0,
		0,0,0,0,0,0,4,1,7,8,
	};
	gcli_pred_t pred_buf[NSAMPLES], pred_buf2[NSAMPLES];
	gcli_data_t gcli_buf_out[NSAMPLES];
	gcli_data_t gcli_buf_top_trunc[NSAMPLES];
	gcli_data_t predictors[NSAMPLES];
	int i, gtli, gtli_top, gtli_top_old;
	int reduce_predictor;

	for (reduce_predictor = 0; reduce_predictor<2; reduce_predictor++)
	{
		for (gtli = 0; gtli <= MAX_GCLI; gtli++)
		{
			for (gtli_top = 0; gtli_top <= MAX_GCLI; gtli_top++)
			{
				tco_pred_ver(gcli_buf, gcli_buf_top, NSAMPLES, pred_buf, predictors, gtli, gtli_top, 0, reduce_predictor);
				tco_pred_ver_inverse(pred_buf, gcli_buf_top_trunc, NSAMPLES, gcli_buf_out, gtli, gtli_top, 0, reduce_predictor);
	
				if (check_result(gcli_buf, gcli_buf_out, pred_buf, NSAMPLES,gtli) == 0)
				{
					fprintf(stderr, "TEST PASSED VER gtli=%d gtli_top=%d\n", gtli, gtli_top);
				}
				else
				{
					fprintf(stderr, "TEST FAILED VER gtli=%d gtli_top=%d\n", gtli, gtli_top);
					fprintf(stderr, "TOP:     ");
					DUMP_BUF(gcli_buf_top,NSAMPLES);
					fprintf(stderr, "\nTOP trunc");
					DUMP_BUF(gcli_buf_top_trunc,NSAMPLES);
					fprintf(stderr, "\nCURRENT: ");
					DUMP_BUF(gcli_buf,NSAMPLES);
					fprintf(stderr, "\nPRED:    ");
					DUMP_BUF(pred_buf,NSAMPLES);
					fprintf(stderr, "\nOUT:     ");
					DUMP_BUF(gcli_buf_out,NSAMPLES);
					return -1;
				}
			}
		}
	}

	for (reduce_predictor = 0; reduce_predictor<2; reduce_predictor++)
	{
		for (gtli = 0; gtli <= MAX_GCLI; gtli++)
		{
			for (gtli_top = 0; gtli_top <= MAX_GCLI; gtli_top++)
			{
				tco_pred_ver(gcli_buf, gcli_buf_top, NSAMPLES, pred_buf,predictors, gtli, gtli_top, 1, reduce_predictor);
				tco_pred_ver_inverse(pred_buf, gcli_buf_top_trunc, NSAMPLES, gcli_buf_out, gtli, gtli_top, 1, reduce_predictor);
	
				if (check_result(gcli_buf, gcli_buf_out, pred_buf, NSAMPLES,gtli) == 0)
				{
					fprintf(stderr, "TEST PASSED VERnew gtli=%d gtli_top=%d\n", gtli, gtli_top);
				}
				else
				{
					fprintf(stderr, "TEST FAILED VERnew gtli=%d gtli_top=%d\n", gtli, gtli_top);
					fprintf(stderr, "TOP:     ");
					DUMP_BUF(gcli_buf_top,NSAMPLES);
					fprintf(stderr, "\nTOP trunc");
					DUMP_BUF(gcli_buf_top_trunc,NSAMPLES);
					fprintf(stderr, "\nCURRENT: ");
					DUMP_BUF(gcli_buf,NSAMPLES);
					fprintf(stderr, "\nPRED:    ");
					DUMP_BUF(pred_buf,NSAMPLES);
					fprintf(stderr, "\nOUT:     ");
					DUMP_BUF(gcli_buf_out,NSAMPLES);
					return -1;
				}
			}
		}
	}	 
	return 0;
}

#endif
