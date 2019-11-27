#pragma once

#include "ICA_smp.h"
#include <CL/cl.hpp>

#include <fstream>


class ICA_opencl : public ICA_smp {
private:
	//init flag
	static bool _INIT;

	static cl::Platform platform;
	static cl::Context context;
	static cl::Device device;
	static cl::Program program;
	static cl::CommandQueue queue;

	//limit to run vector calc on GPU
	size_t cl_size = 0;

	//init OpenCL - platform, device, context, command queue, program
	static void init();

	//perform vector operation on GPU
	std::vector<double> vector_op(std::string op, std::vector<double>& vec1, std::vector<double>& vec2);
	//returns min NDRange size based on work group size and count
	size_t shrRoundUp(size_t localWorkSize, size_t numItems);

protected:
	//calc fitness of colony
	virtual double calc_fitness(const std::vector<double>& vec) override;
	//move colony towards to imperialist - calc on GPU
	virtual void move_colony(Country& imp, Country& colony) override;

public:
	ICA_opencl(const solver::TSolver_Setup& setup);
	~ICA_opencl() = default;

	//add 2 vectors
	virtual std::vector<double> vector_add(std::vector<double>& vec1, std::vector<double>& vec2) override;
	//sub 2 vectors
	virtual std::vector<double> vector_sub(std::vector<double>& vec1, std::vector<double>& vec2) override;
	//multiplies 2 vectors
	virtual std::vector<double> vector_mul(std::vector<double>& vec1, std::vector<double>& vec2) override;
};