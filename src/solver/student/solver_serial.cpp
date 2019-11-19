#include "..\solver_smp.h"
#include "ICA.h"
#include "Statistics.h"

#include<numeric>
#include<tbb/parallel_reduce.h>
#include<tbb/blocked_range.h>

HRESULT solve_serial(solver::TSolver_Setup &setup, solver::TSolver_Progress &progress) {

	//return S_FALSE;

	Statistics::begin(setup, 0);
	ICA ica(setup);


	////////////////////////////test
	/*tbb::concurrent_vector<double> a{ 1,2,3,4,5,6,7,8,9,10 };
	tbb::concurrent_vector<double> b(5);

	size_t size = a.size();
	std::copy(a.end()  - 5, a.end() , b.begin());

	double sum = tbb::parallel_reduce(tbb::blocked_range<tbb::concurrent_vector<double>::iterator>(a.begin(), a.end()), 0.0, [&](const auto& r, auto& init) {
		return std::accumulate(r.begin(), r.end(), init);
		},
		std::plus<double>());

	auto it =  a.begin();
	double e = *it;
	double min = tbb::parallel_reduce(tbb::blocked_range<tbb::concurrent_vector<double>::iterator>(a.begin(), a.end()), 0.0, [&](const auto& r, auto& init) {
		return *std::min_element(a.begin(), a.end(), [](double& a, double& b) { return a < b; });
		},
		[](const double x, const double y) {return x < y; });

	std::cout << min << std::endl;
	system("pause");*/

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