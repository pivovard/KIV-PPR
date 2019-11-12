#include "..\solver_smp.h"
#include "ICA_smp.h"
#include "Statistics.h"


HRESULT solve_smp(solver::TSolver_Setup &setup, solver::TSolver_Progress &progress) {
	
	//return S_FALSE;

	Statistics::begin(setup, 1);
	ICA_smp ica(setup);

	ica.gen_population();
	ica.print_population();

	int i = 0;
	for (i; i < setup.max_generations; ++i) {
		ica.evolve();

		double cost_n = ica.get_min();
		Statistics::iteration(cost_n);

		//end if convergence stopped
		if (false) {
			break;
		}
	}

	ica.write_solution();
	ica.print_population();

	Statistics::end(i);
	Statistics::print_stat();

	if (setup.population_size == 100) {
		Statistics::export_stat("smp");
		Statistics::clear();
	}

	return S_OK;
}