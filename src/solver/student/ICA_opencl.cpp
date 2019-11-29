#include "ICA_opencl.h"

bool ICA_opencl::_INIT = false;
cl::Platform ICA_opencl::platform;
cl::Context ICA_opencl::context;
cl::Device ICA_opencl::device;
cl::Program ICA_opencl::program;
cl::CommandQueue ICA_opencl::queue;

ICA_opencl::ICA_opencl(const solver::TSolver_Setup& setup) : ICA_smp(setup) {
	//init opencl
	if (!_INIT) {
		ICA_opencl::init();
		_INIT = true;
	}
}

void ICA_opencl::init()
{
	cl_int err = 0;
	
	//get platforms
	std::vector<cl::Platform> platformList;
	cl::Platform::get(&platformList);
	std::cout << "Platforms: " << platformList.size() << std::endl;

	if (platformList.size() < 1) {
		throw std::exception("ERROR: No suitable platform!");
	}

	//get vendor of first platform
	std::string platformVendor;
	platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
	std::cout << "First platform is: " << platformVendor << std::endl;

	ICA_opencl::platform = platformList[0];

	cl_context_properties cprops[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0 };
	//get context
	cl::Context context(CL_DEVICE_TYPE_GPU, cprops, NULL, NULL, &err);
	if (err != CL_SUCCESS) {
		throw std::exception("ERROR: Get context failed!");
	}

	ICA_opencl::context = context;

	//get devices
	std::vector<cl::Device> devices;
	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	std::cout << "Devices: " << devices.size() << std::endl;

	if (devices.size() < 1) {
		throw std::exception("ERROR: No suitable device!");
	}

	//load program
	//std::ifstream file("kernel.cl");
	//std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length() + 1));

	cl::Program program(context, source);
	err = program.build(devices);
	if (err != CL_SUCCESS) {
		throw std::exception("ERROR: Failed to load or build program!");
	}

	ICA_opencl::device = devices[0];
	ICA_opencl::program = program;

	cl::CommandQueue queue(ICA_opencl::context, ICA_opencl::device, 0, &err);
	ICA_opencl::queue = queue;
}

std::vector<double> ICA_opencl::vector_add(std::vector<double>& vec1, std::vector<double>& vec2)
{
	if (setup.problem_size < cl_size) return ICA::vector_add(vec1, vec2);
	else return vector_op("vector_add", vec1, vec2);
}

std::vector<double> ICA_opencl::vector_sub(std::vector<double>& vec1, std::vector<double>& vec2)
{
	if (setup.problem_size < cl_size) return ICA::vector_sub(vec1, vec2);
	else return vector_op("vector_sub", vec1, vec2);
}

std::vector<double> ICA_opencl::vector_mul(std::vector<double>& vec1, std::vector<double>& vec2)
{
	if (setup.problem_size < cl_size) return ICA::vector_mul(vec1, vec2);
	else return vector_op("vector_mul", vec1, vec2);
}

double ICA_opencl::calc_fitness(const std::vector<double>& vec)
{
	return ICA::calc_fitness(vec);
	const double mShift = -4.0;
	double result = 0.0;
	for (size_t i = 0; i < setup.problem_size; i++) {
		const double tmp = vec[i] + mShift;
		result -= tmp * sin(sqrt(fabs(tmp)));
	}
	return result;
}

void ICA_opencl::move_colony(Country& imp, Country& colony)
{
	//reandom vector - average 1 in 2 generations
	if (gen_double(0, 1) < 1.0 / (2 * setup.population_size)) {
		colony.vec = gen_vector(setup.problem_size, *setup.lower_bound, *setup.upper_bound);
		colony.fitness = calc_fitness(colony.vec);
		return;
	}
	
	if (setup.problem_size < cl_size) {
		ICA::move_colony(imp, colony);
		return;
	}

	cl_int err = 0;
	const size_t size = setup.problem_size;
	const size_t local_ws = 32; // Number of work-items per workgroup
	// shrRoundUp returns the smallest multiple of local_ws bigger than size
	const size_t global_ws = shrRoundUp(local_ws, size);

	cl::Kernel kernel(program, "move_colony", &err);
	if (err != CL_SUCCESS) {
		throw std::exception("ERROR: Failed to load kernel!");
	}

	std::vector<double> U = gen_vector(setup.problem_size, 0, 1); //U(0,1)
	std::vector<double> res(size);

	cl::Buffer v1(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size * sizeof(double), colony.vec.data(), &err);
	cl::Buffer v2(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size * sizeof(double), imp.vec.data(), &err);
	cl::Buffer u(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size * sizeof(double), U.data(), &err);
	cl::Buffer r(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, size * sizeof(double), res.data(), &err);

	if (err != CL_SUCCESS) {
		throw std::exception("ERROR: Failed to create buffer!");
	}

	err = kernel.setArg(0, v1);
	err |= kernel.setArg(1, v2);
	err |= kernel.setArg(2, u);
	err |= kernel.setArg(3, r);

	if (err != CL_SUCCESS) {
		throw std::exception("ERROR: Failed to set arguments!");
	}

	cl::Event event;
	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global_ws), cl::NDRange(local_ws), NULL, &event);

	if (err != CL_SUCCESS) {
		throw std::exception("ERROR: Failed to enqueue task!");
	}

	event.wait();
	err = queue.enqueueReadBuffer(r, CL_TRUE, 0, size * sizeof(double), res.data());

	if (err != CL_SUCCESS) {
		throw std::exception("ERROR: Failed to read buffer!");
	}

	std::copy(res.begin(), res.end(), colony.vec.begin());
	colony.fitness = calc_fitness(colony.vec);
}

std::vector<double> ICA_opencl::vector_op(std::string op, std::vector<double>& vec1, std::vector<double>& vec2)
{
	cl_int err = 0;
	const size_t size = vec1.size();
	const size_t local_ws = 32; // Number of work-items per workgroup
	// shrRoundUp returns the smallest multiple of local_ws bigger than size
	const size_t global_ws = shrRoundUp(local_ws, size);

	cl::Kernel kernel(program, op.c_str(), &err);
	if (err != CL_SUCCESS) {
		throw std::exception("ERROR: Failed to load kernel!");
	}

	std::vector<double> res(size);

	cl::Buffer v1(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size * sizeof(double), vec1.data(), &err);
	cl::Buffer v2(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size * sizeof(double), vec2.data(), &err);
	cl::Buffer r(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, size * sizeof(double), res.data(), &err);

	if (err != CL_SUCCESS) {
		throw std::exception("ERROR: Failed to create buffer!");
	}

	err = kernel.setArg(0, v1);
	err |= kernel.setArg(1, v2);
	err |= kernel.setArg(2, r);

	if (err != CL_SUCCESS) {
		throw std::exception("ERROR: Failed to set arguments!");
	}

	cl::Event event;
	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global_ws), cl::NDRange(local_ws), NULL, &event);

	if (err != CL_SUCCESS) {
		throw std::exception("ERROR: Failed to enqueue task!");
	}

	event.wait();
	err = queue.enqueueReadBuffer(r, CL_TRUE, 0, size * sizeof(double), res.data());

	if (err != CL_SUCCESS) {
		throw std::exception("ERROR: Failed to read buffer!");
	}

	return res;
}

size_t ICA_opencl::shrRoundUp(size_t localWorkSize, size_t numItems) {
	size_t result = localWorkSize;
	while (result < numItems)
		result += localWorkSize;

	return result;
}