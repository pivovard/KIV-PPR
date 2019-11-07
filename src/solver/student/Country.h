#pragma once
#include <vector>

struct Country
{
	std::vector<double> vec;
	double fitness = std::numeric_limits<double>::quiet_NaN();
	bool imperialist = false;
};

struct Imperialist
{
	Country* imp;
	std::vector<Country*> colonies;
	double total_fitness = std::numeric_limits<double>::quiet_NaN();
};