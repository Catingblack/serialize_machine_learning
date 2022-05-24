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

#include "budget_dump.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "packing.h"
#include "ra.h"
#include "buf_mgmt.h"
#include "precinct.h"


struct budget_dump_struct_t {
	FILE* output_file;  
	int n_lvls;         
	int max_column_size;  

	int scenario2gtli_table[MAX_GCLI + 1][MAX_LVLS];  
	const uint8_t* sb_priority;  


};

 
static const char* get_method_name(int coding_method){
	switch(coding_method){
#if 0
	case GCLI_RAW: return "RAW";
	case GCLI_VER_PRED: return "VP";
	case GCLI_VER_PRED_SIG: return "VSIG";
	case GCLI_VER_PRED_RUN: return "VRUN";
	case GCLI_NO_PRED_UNARY: return "NO_PRED_UNARY"; 	 
	case GCLI_NO_PRED_UNARY_SIG: return "NO_PRED_UNARY_SIG";  
	case GCLI_VER_PRED_SIG_ALTALPH: return "VSIG_ALTALPH";
	case GCLI_METHODS_NB: return NULL;
#endif
	default: return NULL;
	};
}

 
static void budget_dump_header(struct budget_dump_struct_t* handle)
{

	fprintf(handle->output_file, "Prec; ");
	fprintf(handle->output_file, "Scenario; ");
	fprintf(handle->output_file, "Method; ");


	 
	fprintf(handle->output_file, "RA_Scenario; ");
	fprintf(handle->output_file, "RA_Refinement; ");
	fprintf(handle->output_file, "RA_Method; ");


	fprintf(handle->output_file, "Budget");
	fprintf(handle->output_file, "\n");

}



struct budget_dump_struct_t* budget_dump_init(
	const precinct_t *prec, 
	const char* filename, 
	int n_lvls, 
	int scenario2gtli_table[MAX_GCLI + 1][MAX_LVLS],
	const uint8_t* sb_priority
	)
{
	
	struct budget_dump_struct_t* handle = malloc(sizeof(struct budget_dump_struct_t));
	if (handle == NULL) return handle;
	
	handle->n_lvls = n_lvls;
	handle->sb_priority = sb_priority;
	
	for (int scenario = 0; scenario <= MAX_GCLI; scenario++) 
	{
		for (int lvl = 0; lvl < MAX_LVLS; lvl++)
		{
			handle->scenario2gtli_table[scenario][lvl] = scenario2gtli_table[scenario][lvl];
		}
	}


	handle->output_file = fopen(filename, "w");
	if (handle->output_file == NULL){
		fprintf(stderr,"Could not open budget dump file\n");
		budget_dump_free(handle);
		handle = NULL;
	}else{
		budget_dump_header(handle);
	}

	
	return handle;
}


void budget_dump_free(struct budget_dump_struct_t* handle)
{
	if (handle == NULL) return;
	if (handle->output_file != NULL)
		fclose(handle->output_file);


	free(handle);
}

void budget_dump_write_GCLI(
	struct budget_dump_struct_t* handle, 
	int precinct_line_nb, 
	int selected_scenario, 
	int selected_refinement, 
	const multi_buf_t* gcli_budget_table, 
	int gcli_method_ra)
{

	if (handle == NULL) return;

	int gcli_method;
	int scenario;
	int lvl;

	 
	scenario = MAX_GCLI;
	lvl = handle->n_lvls-1;


	 
	for (scenario = 0; scenario <= MAX_GCLI; scenario++) {

#define LOOP_DELTA 0


		for (gcli_method = 0; gcli_method < GCLI_METHODS_NB + LOOP_DELTA; gcli_method++) {

			fprintf(handle->output_file, "%d; ", precinct_line_nb);
			fprintf(handle->output_file, "%d; ", scenario);
			fprintf(handle->output_file, "%s; ", get_method_name(gcli_method));


			 
			fprintf(handle->output_file, "%d; ", selected_scenario);
			fprintf(handle->output_file, "%d; ", selected_refinement);
			fprintf(handle->output_file, "%s; ", get_method_name(gcli_method_ra));




			int32_t lvl_sum = 0;
			for(lvl = 0; lvl < handle->n_lvls; lvl++){
				const int gtli = handle->scenario2gtli_table[scenario][lvl];
				int32_t gcli_budget;
				if (gcli_method < GCLI_METHODS_NB)
				{
					 
					gcli_budget = (int32_t)(gcli_budget_table->bufs.u32[idx2d(gcli_method, lvl, handle->n_lvls)][gtli]);
					if (gcli_budget == RA_BUDGET_INVALID)
					{
						gcli_budget = -1;
					}
					else
					{
						lvl_sum += gcli_budget;

					}
				}

			}
			fprintf(handle->output_file, "%d ", lvl_sum);



			fprintf(handle->output_file, "\n");
		}		
	}
}



void budget_dump_write(
	struct budget_dump_struct_t* handle,
	int precinct_line_nb,
	int selected_scenario,
	int selected_refinement,
	const multi_buf_t* gcli_budget_table,
	const multi_buf_t* raw_budget_table,
	int gcli_method_ra)
{
	budget_dump_write_GCLI(handle, precinct_line_nb, selected_scenario, selected_refinement, gcli_budget_table, gcli_method_ra);
}
