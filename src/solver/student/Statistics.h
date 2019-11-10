#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include "../../common/iface/SolverIface.h"


struct Stat
{
	double time = 0.0;
	int generations = 0;
	std::vector<double> fitness;
	size_t population_size;
	size_t problem_size;
	short type;
};

static class Statistics
{
private:
	static std::vector<Stat> stats;
	static time_t start_time;

public:
	static void begin(solver::TSolver_Setup& setup, short type);
	static void iteration(double cost);
	static void end(int gen);
	static void print_stat();
	static void export_stat();
};

