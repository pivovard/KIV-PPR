#include "..\solver_smp.h"
#include "ICA.h"
#include "Statistics.h"

#include<numeric>

HRESULT solve_serial(solver::TSolver_Setup &setup, solver::TSolver_Progress &progress) {

	//return S_FALSE;

	Statistics::begin(setup, 0);
	ICA ica(setup);


	////////////////////////////test
	std::vector<double> a{ 1,2,3,4,5,6,7,8,9,10 };
	std::vector<double> b(5);
	//b = std::vector<double>(a.size() - 1);
	size_t size = a.size();
	std::copy(a.end()  - 5, a.end() , b.begin());

	ica.print_vector(a);
	ica.print_vector(b);

	system("pause");

	////////////////////////endtest

	ica.gen_population();
	ica.print_population();

	int i = 0;
	for (i; i < setup.max_generations; ++i) {
		ica.evolve();

		double cost_n = ica.get_min();
		Statistics::iteration(cost_n);

		//end if convergence stopped
		size_t n = 10;
		double eps = 0.0000000001;
		if (i > n) {
			std::vector<double> vec = Statistics::get_last_n(n);
			double sum = std::accumulate(vec.begin(), vec.end(), 0.0);

			if (std::abs(sum / n - vec.back()) < eps) break;
		}
	}

	ica.write_solution();
	ica.print_population();

	Statistics::end(i);
	Statistics::print_stat();

	if (setup.population_size == 100) {
		Statistics::export_stat("serial");
		Statistics::clear();
	}

	system("pause");
	return S_OK;
}