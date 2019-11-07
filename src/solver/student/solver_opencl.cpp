#include "..\solver_opencl.h"
#include <iostream>

HRESULT solve_opencl(solver::TSolver_Setup &setup, solver::TSolver_Progress &progress) {


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