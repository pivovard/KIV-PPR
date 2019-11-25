#include "Statistics.h"



std::vector<Stat> Statistics::stats;
std::chrono::time_point<std::chrono::steady_clock> Statistics::start_time;
short Statistics::problem_n = 0;


void Statistics::begin(const solver::TSolver_Setup& setup, short type)
{
	start_time = std::chrono::steady_clock::now();

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

std::vector<double> Statistics::get_last_n(size_t n)
{
	Stat& stat = Statistics::stats.back();

	std::vector<double> vec(n);
	std::copy(stat.fitness.end() - n, stat.fitness.end(), vec.begin());

	return vec;
}

void Statistics::end(int gen)
{
	auto end_time = std::chrono::steady_clock::now();

	Stat& stat = Statistics::stats.back();
	auto diff = end_time - start_time;
	stat.time = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
	stat.generations = gen;
}

void Statistics::clear() {
	Statistics::stats.clear();
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
			<< ", generations: " << stat.generations << ", solution: " << stat.fitness.back() << ", time: " << stat.time << "ms" << std::endl;
	}
}

void Statistics::export_stat(std::string type)
{
	std::ofstream file;
	std::ofstream file_csv;
	std::ofstream file_all;
	if (problem_n > 0) { //append
		file.open("results_" + type + ".txt", std::ofstream::out | std::ofstream::app);
		file_all.open("results_all_" + type + ".txt", std::ofstream::out | std::ofstream::app);
		file_csv.open("results_" + type + ".csv", std::ofstream::out | std::ofstream::app);
		file << std::endl;
		file_all << std::endl;
		file_csv << std::endl;
	}
	else { //new
		file.open("results_" + type + ".txt", std::ofstream::out | std::ofstream::trunc);
		file_all.open("results_all_" + type + ".txt", std::ofstream::out | std::ofstream::trunc);
		file_csv.open("results_" + type + ".csv", std::ofstream::out | std::ofstream::trunc);
	}

	problem_n++;
	
	for (Stat& stat : Statistics::stats) {
		switch (stat.type) {
		case 0: 
			file << "Type: serial";
			file_all << "Type: serial";
			break;
		case 1: 
			file << "Type: smp";
			file_all << "Type: smp";
			break;
		case 2: 
			file << "Type: openCL";
			file_all << "Type: openCL";
			break;
		}

		file << ", problem size: " << stat.problem_size << ", population size: " << stat.population_size
			<< ", generations: " << stat.generations << ", solution: " << stat.fitness.back() << ", time: " << stat.time << "ms" << std::endl;
		file_all << ", problem size: " << stat.problem_size << ", population size: " << stat.population_size
			<< ", generations: " << stat.generations << ", solution: " << stat.fitness.back() << ", time: " << stat.time << "ms" << std::endl;

		file_csv << stat.type << "," << stat.problem_size << "," << stat.population_size << "," << stat.generations << "," << stat.fitness.back() << "," << stat.time <<std::endl;

		for (auto& cost : stat.fitness) {
			file_all << cost << ",";
		}
		file_all << std::endl << std::endl;
	}



	file.close();
}
