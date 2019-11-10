#include "Statistics.h"



std::vector<Stat> Statistics::stats;
time_t Statistics::start_time;


void Statistics::begin(solver::TSolver_Setup& setup, short type)
{
	time(&start_time);

	Stat stat;
	stat.type = type;
	stat.problem_size = setup.problem_size;
	stat.population_size = setup.population_size;

	Statistics::stats.push_back(stat);
}

void Statistics::iteration(double cost)
{
	Stat& stat = Statistics::stats.back();
	stat.fitness.push_back(cost);
}

void Statistics::end(int gen)
{
	time_t end_time;
	time(&end_time);

	Stat& stat = Statistics::stats.back();
	stat.time = difftime(end_time, start_time);
	stat.generations = gen;
}


void Statistics::print_stat()
{
	for (Stat& stat : Statistics::stats) {
		switch (stat.type) {
		case 0: std::cout << "Type: serial"; break;
		case 1: std::cout << "Type: smp"; break;
		case 2: std::cout << "Type: openCL"; break;
		}

		std::cout << ", problem size: " << stat.problem_size << ", population size: " << stat.population_size 
			<< ", generations: " << stat.generations << ", time: " << stat.time << "s" << std::endl;
	}
}

void Statistics::export_stat()
{
	std::ofstream file;
	file.open("results.txt");

	for (Stat& stat : Statistics::stats) {
		switch (stat.type) {
		case 0: file << "Type: serial"; break;
		case 1: file << "Type: smp"; break;
		case 2: file << "Type: openCL"; break;
		}

		file << ", problem size: " << stat.problem_size << ", population size: " << stat.population_size
			<< ", generations: " << stat.generations << ", time: " << stat.time << "s" << std::endl;

		for (auto& cost : stat.fitness) {
			file << cost << ",";
		}

		file << std::endl;
	}

	file.close();
}
