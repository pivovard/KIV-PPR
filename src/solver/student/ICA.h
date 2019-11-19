#pragma once

#include <list>
#include <vector>
#include <iostream>
#include<random>
#include<tbb/concurrent_vector.h>

#include "../../common/iface/SolverIface.h"
#include "Country.h"



class ICA
{
private:

protected:
	const solver::TSolver_Setup& setup;

	tbb::concurrent_vector<Country> pop;
	tbb::concurrent_vector<Imperialist> imp;

	const double beta = 2.0;
	const double gama = std::_Pi / 4.0;
	const double xi = 0.5;

	const size_t start_imp = 3;
	const size_t max_colonies = 12;

	std::random_device rd; // obtain a random number from hardware
	std::mt19937 eng; // seed the generator
	static thread_local std::mt19937_64 eng64;
	

public:

	ICA(const solver::TSolver_Setup& setup);
	~ICA() = default;

	virtual void gen_population();
	virtual void evolve();
	virtual void move_all_colonies(Imperialist& imp);
	virtual void migrate_colonies();

	virtual void calc_fitness_all();
	virtual double calc_fitness_imp(const Imperialist& imp);

	

	void move_colony(Country& imp, Country& colony);
	void do_migration(const std::vector<double>& P, const std::vector<std::vector<_int64>>& migration);
	void do_migration(const std::vector<double>& P, const tbb::concurrent_vector<std::vector<_int64>>& migration);

	double get_min();
	double get_max();
	double calc_fitness(const std::vector<double>& vec);
	double calc_distance(const std::vector<double>& vec1, const std::vector<double>& vec2);
	void write_solution();

	double gen_double(double lower_bound, double upper_bound);
	std::vector<double> gen_vector(size_t size, double lower_bound, double upper_bound);

	void print_vector(const std::vector<double>& vec);
	void print_population();
};