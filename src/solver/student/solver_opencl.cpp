#include "..\solver_opencl.h"
#include <iostream>

HRESULT solve_opencl(solver::TSolver_Setup &setup, solver::TSolver_Progress &progress) {

	
	//az ho sem naimplementujete, pak se vrati S_OK, S_FALSE, E_INVALIDARG nebo E_FAIL dle duvod, proc volani solveru selhalo
					  //https://docs.microsoft.com/en-us/windows/win32/learnwin32/error-handling-in-com

	//kdyz uspesne najdete reseni, zapisete ho do setup.solution

	//std::cout << "Kde to kurva jsme??" << std::endl;
	std::cout << setup.problem_size << std::endl;
	std::cout << setup.population_size << std::endl;
	std::cout << setup.max_generations << std::endl;
	std::cout << setup.objective << std::endl;
	
	//double res = Calculate_Fitness(setup.solution, setup.problem_size);
	//std::cout << res << std::endl;
	system("pause");

	return S_FALSE;
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