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
	//for (int i = 0; i < setup.max_generations; i++) {
	for (int i = 0; i < 10; i++) {
		//move colonies
		for (auto& i : imp) {
			move_all_colonies(i);
		}
		//calc_fitness_all();

		migrate_colonies();
	}
}

void ImperialistAlg::move_all_colonies(Imperialist& imp)
{
	for (int i = 0; i < imp.colonies.size(); i++) {
		move_colony(*imp.imp, *imp.colonies[i]);

		//calc new cost
		imp.colonies[i]->fitness = calc_fitness(imp.colonies[i]->vec);

		//if colonies cost function < than imperialists, switch
		if (imp.colonies[i]->fitness < imp.imp->fitness) {
			auto* tmp = imp.colonies[i];
			imp.colonies[i] = imp.imp;
			imp.colonies[i]->imperialist = false;
			imp.imp = tmp;
			imp.imp->imperialist = true;
		}
	}
}

void ImperialistAlg::move_colony(Country& imp, Country& colony)
{
	//x - U(0,beta*d) - nefunkcni, pravdepodobne smerovy vector je spatne
	//double d = calc_distance(imp.vec, colony.vec);
	//double U = gen_double(0, beta * d); //U(0,beta*d)
	std::vector<double> U = gen_vector(setup.problem_size, 0, 1); //U(0,1)

	//V=imp-col
	std::vector<double> V(setup.problem_size);
	std::transform(imp.vec.begin(), imp.vec.end(), colony.vec.begin(), V.begin(), std::minus<double>());

	//V=V*U
	//for (auto& v : V) v *= U;
	std::transform(V.begin(), V.end(), U.begin(), V.begin(), std::multiplies<double>());

	//Xnew=Xold+V*U
	std::transform(colony.vec.begin(), colony.vec.end(), V.begin(), colony.vec.begin(), std::plus<double>());

	//x - U(-gama,gama)
	//double O = gen_double(-gama, gama); //U(-gama, gama)

	//count new fitness
	colony.fitness = calc_fitness(colony.vec);
}

void ImperialistAlg::migrate_colonies()
{
	double max_tc = DBL_MIN;
	double sum_tc = 0.0;

	//count total fitness
	for (auto& i : imp) {
		double tc = calc_fitness_imp(i);
		i.total_fitness = tc;
		sum_tc += tc;
		if (tc > max_tc) max_tc = tc;
	}

	//count probability vector p=|NTC/sum NTC |
	std::vector<double> P;

	for (auto& i : imp) {
		std::cout << i.total_fitness << std::endl;
		double p = std::abs((i.total_fitness - max_tc)/sum_tc);
		P.push_back(p);
	}

	std::vector<double> R;
	R = gen_vector(P.size(), 0, 1);

	//D=P-R
	std::vector<double> D(P.size());
	std::transform(P.begin(), P.end(), R.begin(), D.begin(), std::minus<double>());

	std::cout << P[0] << " - " << R[0]<< " = " << D[0] << std::endl;
	std::cout << P[1] << " - " << R[1] << " = " << D[1] << std::endl;


	//imperialist losts power
	for (auto& i : imp) {
		if (i.colonies.size() == 0) {

		}
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

double ImperialistAlg::gen_double(double lower_bound, double upper_bound)
{
	std::uniform_real_distribution<> distr(lower_bound, upper_bound);
	return distr(eng);
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

double ImperialistAlg::calc_fitness_imp(const Imperialist& imp)
{
	double sum = imp.imp->fitness;
	for (int i = 0; i < imp.colonies.size(); i++) {
		sum += imp.colonies[i]->fitness;
	}

	return sum;
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
		//std::cout << std::endl;
	}
}

void ImperialistAlg::print_vector(const std::vector<double>& vec)
{
	for (const auto& v : vec) {
		std::cout << v << " ";
	}
	std::cout << std::endl;
}
