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

	//for (int i = n_imp; i < setup.population_size; ++i) {
	//tbb::parallel_for(tbb::blocked_range<size_t>(n_imp, setup.population_size), [&](const auto& r) {
	tbb::parallel_for(size_t(n_imp), setup.population_size, [&](size_t r) {
		double roll = dist(eng);
		double tmp = 0;

		for (int j = 0; j < n_imp; ++j) {
			tmp += prob[j];
			if (roll <= tmp) {
				//std::cout << pop.size(); //dunno why this works (maybe console output is synchronized) - because colonies were std::vector, not parallel
				imp[j].colonies.push_back(&pop[r]);
				break;
			}
		}
	});
}

void ICA_smp::evolve()
{
	//move colonies
	tbb::parallel_for(size_t(0), imp.size(), [&](size_t r) {
		move_all_colonies(imp[r]);
	});

	if (imp.size() > 1) {
		//migrate colonies
		migrate_colonies();
	}
}

void ICA_smp::move_all_colonies(Imperialist& imp)
{
	//for (int i = 0; i < imp.colonies.size(); ++i) {
	tbb::parallel_for(size_t(0), imp.colonies.size(), [&](size_t r) {
		move_colony(*imp.imp, *imp.colonies[r]);

		//calc new cost - already at move_colony
		//imp.colonies[i]->fitness = calc_fitness(imp.colonies[i]->vec);

		//if colonies cost function < than imperialists, switch
		if (imp.colonies[r]->fitness < imp.imp->fitness) {
			auto* tmp = imp.colonies[r];
			imp.colonies[r] = imp.imp;
			imp.colonies[r]->imperialist = false;
			imp.imp = tmp;
			imp.imp->imperialist = true;
		}
	});
}

void ICA_smp::migrate_colonies()
{
	//double max_tc = DBL_MIN;
	double max_tc = get_max();
	double sum_tc = 0.0;
	tbb::mutex mutex;

	//count total fitness
	//for (auto& i : imp) {
	tbb::parallel_for(size_t(0), imp.size(), [&](size_t r) {
		double tc = calc_fitness_imp(imp[r]);
		imp[r].total_fitness = tc;
		tbb::mutex::scoped_lock lock(mutex);
		sum_tc += tc;
	});

	//count probability vector p=|NTC/sum NTC |
	std::vector<double> P;

	for (auto& i : imp) {
		//double p = std::abs((i.total_fitness - max_tc) / sum_tc); //normalized
		double p = std::abs(i.total_fitness / sum_tc); // not normalized
		P.push_back(p);
	}

	//{colony, imp in, imp out}
	//{  j,      max,     i   }
	std::vector<std::vector<_int64>> migration;
	//for (int i = 0; i < imp.size(); ++i) {
	tbb::parallel_for(size_t(0), imp.size(), [&](size_t i) {
		//for (auto* col : imp[i].colonies) {
		//for (int j = 0; j < imp[i].colonies.size(); ++j) {
		for (int j = imp[i].colonies.size() - 1; j > -1; --j) {
			std::vector<double> R;
			R = gen_vector(P.size(), 0, 1);

			//D=P-R
			std::vector<double> D(P.size());
			std::transform(P.begin(), P.end(), R.begin(), D.begin(), std::minus<double>());

			auto it = std::min_element(D.begin(), D.end());
			_int64 max = std::distance(D.begin(), it);

			//migration
			if (max != i) {
				//imp[max].colonies.push_back(imp[i].colonies[j]);
				//imp[i].colonies.erase(imp[i].colonies.begin()+j);
				tbb::mutex::scoped_lock lock(mutex);
				migration.push_back({ j, max, _int64(i) });
			}
		}
	});
	//std::cout << migration.size();
	do_migration(P, migration);
}

void ICA_smp::calc_fitness_all()
{
	/*for (auto& country : pop) {
		country.fitness = calc_fitness(country.vec);
	}*/
	tbb::parallel_for(size_t(0), setup.population_size, [&](size_t r) {
		pop[r].fitness = calc_fitness(pop[r].vec);
	});
}

