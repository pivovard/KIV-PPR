﻿#include "..\solver_opencl.h"
#include "ICA_opencl.h"
#include "ICA_opencl2.h"
#include "Statistics.h"


HRESULT solve_opencl(solver::TSolver_Setup& setup, solver::TSolver_Progress& progress) {

	//return S_FALSE;

	try {
		ICA_opencl2 ica(setup);
		
		Statistics::begin(setup, 2);

		ica.gen_population();
		//ica.print_population();

		int i = 0;
		size_t n = 50;
		double eps = 0.000000001;
		for (i; i < 5; ++i) {
			if (progress.cancelled) return S_FALSE;
			
			ica.evolve();

			double cost_n = ica.get_min();
			Statistics::iteration(cost_n);
			
			//end if convergence stopped
			if (i > n) {
				std::vector<double> vec = Statistics::get_last_n(n);
				double sum = std::accumulate(vec.begin(), vec.end(), 0.0);

				if (std::abs(sum / n - vec.back()) < eps) break;
			}
		}

		ica.write_solution();
		//ica.print_population();

		Statistics::end(i);
		//Statistics::print_stat();

		if (setup.population_size == 100) {
			ica.finalize();

			Statistics::export_stat();
			Statistics::clear();
		}
	}
	catch (std::exception & ex) {
		std::cout << ex.what() << std::endl;
		return E_FAIL;
	}

	//system("pause");
	return S_OK;
}