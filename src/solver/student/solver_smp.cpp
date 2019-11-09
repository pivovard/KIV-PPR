#include "..\solver_smp.h"
#include<list>
#include<vector>
#include <iostream>
#include<random>
#include "ImperialistAlg.h"
#include "Statistics.h"


HRESULT solve_smp(solver::TSolver_Setup &setup, solver::TSolver_Progress &progress) {

	Statistics::begin(setup, 1);

	ImperialistAlg imp(setup);

	imp.gen_population();
	imp.print_population();
	
	int i = 0;
	double eps = 0.01;
	for (i; i < setup.max_generations; ++i) {
	//for (i; i < 1000; ++i) {
		imp.evolve();
		imp.print_population();

		if (imp.get_min() < eps) {

		}
	}

	imp.write_solution();

	Statistics::end(i);
	Statistics::print_stat();

	system("pause");
	return S_OK;
}