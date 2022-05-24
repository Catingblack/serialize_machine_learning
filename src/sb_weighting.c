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
#include <assert.h>

#include "common.h"
#include "packing.h"


static INLINE int compute_gtli(int scenario, int gain, int add_1bp)
{
	int gtli = scenario - gain;
	if (add_1bp) 
		gtli -= 1;
	gtli = MAX(gtli, 0);
	gtli = MIN(gtli, MAX_GCLI);
	return gtli;
}

 
int compute_gtli_tables_all_scenarios(
	int n_lvls,
	uint8_t* sb_gains,
	int use_gcli_trunc,
	int scenario2gtli_table[MAX_GCLI + 1][MAX_LVLS])
{
	int lvl;
	int scenario;
	for (scenario = 0; scenario <= MAX_GCLI; scenario++)
	{
		for (lvl = 0; lvl < n_lvls; lvl++)
		{
			scenario2gtli_table[scenario][lvl] = (!use_gcli_trunc) ? 0 : compute_gtli(scenario, sb_gains[lvl], 1);
		}

		for (lvl = n_lvls; lvl < MAX_LVLS; lvl++)
		{
			scenario2gtli_table[scenario][lvl] = -1;
		}
	}
	return 0;
}





int compute_gtli_tables(int scenario, 
						int refinement, 
						int n_lvls, 
						uint8_t* sb_gains, 
						uint8_t* sb_priority, 
						int gc_trunc,
						int gc_trunc_use_priority,
						int* gtli_table_data,
						int* gtli_table_gcli,
						int* precinct_empty)
{
	int lvl;
	int lvls_empty = 0; 
	for (lvl=0; lvl<n_lvls; lvl++)
	{
		gtli_table_data[lvl] = compute_gtli(scenario, sb_gains[lvl], (sb_priority[lvl]<refinement));
		if (!gc_trunc)
			gtli_table_gcli[lvl] = 0;
		else if (gc_trunc_use_priority)
			gtli_table_gcli[lvl] = gtli_table_data[lvl];
		else
			gtli_table_gcli[lvl] = compute_gtli(scenario, sb_gains[lvl],1);

		if (gtli_table_data[lvl] == MAX_GCLI)
			lvls_empty++;
	}
	*precinct_empty = (lvls_empty == n_lvls);  
	return 0;
}

