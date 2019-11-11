#include "ICA_smp.h"

ICA_smp::ICA_smp(const solver::TSolver_Setup& setup) : ICA(setup) {}

void ICA_smp::gen_population()
{
	//generate start population
	/*for (int i = 0; i < setup.population_size; ++i) {
		Country c;
		c.vec = gen_vector(setup.problem_size, *setup.lower_bound, *setup.upper_bound);
		pop.push_back(c);
	}*/

	tbb::parallel_for(tbb::blocked_range<size_t>(0, setup.population_size), [&](const auto& r) {
			Country c;
			c.vec = gen_vector(setup.problem_size, *setup.lower_bound, *setup.upper_bound);
			pop.push_back(c);
		});

	//calc fitness functions and sort
	calc_fitness_all();
	std::sort(pop.begin(), pop.end(), [](Country a, Country b) { return a.fitness < b.fitness; });

	//define imperialists
	size_t n_imp = start_imp;
	while (setup.population_size / n_imp >= max_colonies) {
		n_imp++;
	}

	double sum_C = 0; //sum of normalized fitnesses of imperialists
	double max_c = pop[setup.population_size - 1].fitness; //max of all countries OR maybe max of imperialists?

	for (int i = 0; i < n_imp; ++i) {
		Imperialist tmp;
		tmp.imp = &pop[i];
		imp.push_back(tmp);
		pop[i].imperialist = true;
		sum_C += pop[i].fitness - max_c;
	}

	std::vector<double> prob; //probability to get colony

	for (int i = 0; i < n_imp; ++i) {
		double p_n = std::abs((pop[i].fitness - max_c) / sum_C); //p=|norm.fitness/sum Cn|
		prob.push_back(p_n);
		std::cout << "Imp " << i + 1 << " colonies " << std::round(p_n * (setup.population_size - n_imp)) << std::endl;
	}

	//assign colonies - not match number of colonies by the formula
	//tbb::paralel_for ??
	std::uniform_real_distribution<> dist; //distrinution (0,1)

	for (int i = n_imp; i < setup.population_size; ++i) {
		double roll = dist(eng);
		double tmp = 0;

		for (int j = 0; j < n_imp; ++j) {
			tmp += prob[j];
			if (roll <= tmp) {
				imp[j].colonies.push_back(&pop[i]);
				break;
			}
		}
	}
}

