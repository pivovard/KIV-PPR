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
	size_t cl_size = 30;

	//init OpenCL
	static void init();

	std::vector<double> vector_op(std::string op, std::vector<double>& vec1, std::vector<double>& vec2);
	size_t shrRoundUp(size_t localWorkSize, size_t numItems);

protected:
	virtual double calc_fitness(const std::vector<double>& vec) override;

public:
	ICA_opencl(const solver::TSolver_Setup& setup);
	~ICA_opencl() = default;

	virtual std::vector<double> vector_add(std::vector<double>& vec1, std::vector<double>& vec2) override;
	virtual std::vector<double> vector_sub(std::vector<double>& vec1, std::vector<double>& vec2) override;
	virtual std::vector<double> vector_mul(std::vector<double>& vec1, std::vector<double>& vec2) override;
};