#include "..\solver_smp.h"
#include<list>
#include<vector>
#include <iostream>
#include<random>
#include "ImperialistAlg.h"
#include "Statistics.h"


HRESULT solve_smp(solver::TSolver_Setup &setup, solver::TSolver_Progress &progress) {
	
	return S_FALSE;

	Statistics::begin(setup, 1);
	ImperialistAlg imp(setup);

	imp.gen_population();
	imp.print_population();
	
	int i = 0;
	//for (i; i < setup.max_generations; ++i) {
	for (i; i < 10; ++i) {
		imp.evolve();
		imp.print_population();

		double cost_n = imp.get_min();
		Statistics::iteration(cost_n);

		//end if convergence stopped
		if (false) {
			break;
		}
	}

	imp.write_solution();
	imp.print_population();

	Statistics::end(i);
	Statistics::print_stat();

	system("pause");
	return S_OK;
}