#include"ImperialistAlg.h"

std::random_device rd; // obtain a random number from hardware
std::mt19937 eng(rd()); // seed the generator


ImperialistAlg::ImperialistAlg(const solver::TSolver_Setup& setup) : setup(setup)
{
	std::cout << "Problem size:\t" << setup.problem_size << std::endl;
	std::cout << "Population size:\t" << setup.population_size << std::endl;
	std::cout << "Upper bound:\t" << *setup.upper_bound << std::endl;
	std::cout << "Lower bound:\t" << *setup.lower_bound << std::endl;
}


void ImperialistAlg::init()
{
	gen_population();
	print_population();
}

void ImperialistAlg::evolve()
{
	for (int i = 0; i < setup.max_generations; i++) {

	}
}

void ImperialistAlg::write_solution()
{
	/*
	std::vector<double> fitness = calc_fitness();
	auto it = std::min_element(fitness.begin(), fitness.end());
	_int64 index = std::distance(fitness.begin(), it);
	*/

	calc_fitness_all();
	std::sort(pop.begin(), pop.end(), [](Country a, Country b) { return a.fitness < b.fitness; });

	std::copy(pop.front().vec.begin(), pop.front().vec.end(), setup.solution);
}

std::vector<double> ImperialistAlg::gen_vector(size_t size, double lower_bound, double upper_bound)
{
	std::uniform_real_distribution<> distr(lower_bound, upper_bound);

	std::vector<double> vec;

	for (int j = 0; j < size; j++) {
		double val = distr(eng);
		vec.push_back(val);
	}

	return vec;
}

void ImperialistAlg::gen_population()
{
	//generate start population
	for (int i = 0; i < setup.population_size; i++) {
		Country c;
		c.vec = gen_vector(setup.problem_size, *setup.lower_bound, *setup.upper_bound);
		pop.push_back(c);
	}

	//calc fitness functions and sort
	calc_fitness_all();
	std::sort(pop.begin(), pop.end(), [](Country a, Country b) { return a.fitness < b.fitness; });
	
	//define imperialists
	size_t n_imp = 2;
	while (setup.population_size / n_imp >= 20) {
		n_imp++;
	}

	double sum_C = 0; //sum of normalized fitnesses
	double max_c = pop[setup.population_size - 1].fitness;

	for (int i = 0; i < n_imp; i++) {
		Imperialist tmp;
		tmp.imp = &pop[i];
		imp.push_back(tmp);
		pop[i].imperialist = true;
		sum_C += pop[i].fitness - max_c;
	}
	
	std::vector<double> prob; //probability to get colony

	for (int i = 0; i < n_imp; i++) {
		double p_n = std::abs((pop[i].fitness - max_c) / sum_C); //p=|norm.fitness/sum Cn|
		prob.push_back(p_n);
		std::cout << "Imp " << i+1 << " colonies " << std::round(p_n * (setup.population_size - n_imp)) << std::endl;
	}

	//assign colonies - not match number of colonies by the formula
	//tbb::paralel_for ??
	std::uniform_real_distribution<> dist; //distrinution (0,1)
	
	for (int i = n_imp; i < setup.population_size; i++) {
		double roll = dist(eng);
		double tmp = 0;

		for (int j = 0; j < n_imp; j++) {
			tmp += prob[j];
			if (roll <= tmp) {
				imp[j].colonies.push_back(&pop[i]);
				break;
			}
		}
	}
	std::cout << "Actual Imp " << "1" << " colonies " << imp[0].colonies.size() << std::endl;
	std::cout << "Actual Imp " << "2" << " colonies " << imp[1].colonies.size() << std::endl;
}

double ImperialistAlg::calc_fitness(const std::vector<double>& vec)
{
	double res = setup.objective(setup.data, vec.data());
	return res;
}

void ImperialistAlg::calc_fitness_all()
{
	for (auto& country : pop) {
		country.fitness = calc_fitness(country.vec);
	}
}

double ImperialistAlg::calc_distance(const std::vector<double>& vec1, const std::vector<double>& vec2)
{
	double sum = 0.0;

	for (int i = 0; i < vec1.size(); i++) {
		sum += std::pow(vec1[i] - vec2[i], 2);
	}

	return std::sqrt(sum);
}

void ImperialistAlg::print_population()
{
	for (const auto& country : pop) {
		if(country.imperialist) std::cout << "Imp: ";
		else std::cout << "Col: ";
		std::cout << country.fitness << " - ";
		print_vector(country.vec);
		std::cout << std::endl;
	}
}

void ImperialistAlg::print_vector(const std::vector<double>& vec)
{
	for (const auto& v : vec) {
		std::cout << v << " ";
	}
}
