#include "..\solver_opencl.h"
#include "ICA.h"
#include "Statistics.h"

#include <CL/cl.hpp>

HRESULT solve_opencl(solver::TSolver_Setup &setup, solver::TSolver_Progress &progress) {

	size_t m_size = 1024;

	std::vector<int>* vec1 = new std::vector<int>(m_size);
	std::vector<int>* vec2 = new std::vector<int>(m_size);
	std::vector<int>* vec3 = new std::vector<int>(m_size);
	//std::vector<double> vec1(m_size);
	//std::vector<double> vec2(m_size);
	//std::vector<double> vec(m_size);

	for (int i = 0; i < m_size; ++i) {
		vec1[i] = i;
		vec2[i] = m_size - i;
	}
	
	cl_int error = 0;
	cl_platform_id platform;
	cl_context context;
	cl_command_queue queue;
	cl_device_id device;	
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	
	// Platform
	error = clGetPlatformIDs(0, NULL, &ret_num_platforms);
	if (error != CL_SUCCESS) return E_FAIL;
	error = clGetPlatformIDs(ret_num_platforms, &platform, NULL);
	if (error != CL_SUCCESS) return E_FAIL;
	// Device - CL_DEVICE_TYPE_GPU je typ zařízení,1 znamená, že chceme deskriptor 1x GPU
	error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	if (error != CL_SUCCESS) return E_FAIL;
	// Context – zjistili jsme 1x GPU,vytvoříme kontext 1x GPU,ale lze dát i více devices
	context = clCreateContext(0, 1, &device, NULL, NULL, &error);
	if (error != CL_SUCCESS)return E_FAIL;
	// Command-queue – fronta, do které budeme //zadávat příkazy, co se má vypočítat
	queue = clCreateCommandQueue(context, device, 0, &error);
	if (error != CL_SUCCESS) return E_FAIL;

	//float* src_a_h = new float[10];
	//cl_mem src_a_d = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, src_a_h, &error);
	cl_mem mvec1 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, m_size, &vec1, &error);
	cl_mem mvec2 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, m_size, &vec2, &error);
	cl_mem mvec3 = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, m_size, &vec, &error);


	std::ifstream file("kernel.cl");
	std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));

	//cl_program_sources source(1, make_pair(prog.c_str(), prog.length() + 1));
	//cl_program program(context, source);
	file.close();

	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&prog, (const size_t*)(prog.size()), &error);
	error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	
	cl_kernel kernel = clCreateKernel(program, "vector_add", &error);
	
	error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &mvec1);
	error |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &mvec2);
	error |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &mvec3);
	//ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,LIST_SIZE * sizeof(int), A, 0, NULL, NULL);

	size_t global_item_size = m_size; // Process the entire lists
	size_t local_item_size = 64; // Divide work items into groups of 64
	error = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);	

	for (int i = 0; i < m_size; i++)
		std::cout << vec1[i] << " + " << vec2[i] << " = " << vec[i] << std::endl;
	
	clFlush(queue);
	clFinish(queue);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseMemObject(mvec1);
	clReleaseMemObject(mvec2);
	clReleaseMemObject(mvec3);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);


	/*if (setup.population_size == 100) {
		Statistics::export_stat("serial");
		Statistics::clear();
	}*/
	system("pause");
	return S_OK;
}

double Calculate_Fitness(const double* solution, const size_t mProblem_Size) {
	const double mShift = -4.0;
	double result = 0.0;
	for (size_t i = 0; i < mProblem_Size; i++) {
		const double tmp = solution[i] + mShift;
		result -= tmp * sin(sqrt(fabs(tmp)));
	}
	return result;
}