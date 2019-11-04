#pragma once

#include "..\common\iface\SolverIface.h"

HRESULT solve_opencl(solver::TSolver_Setup &setup, solver::TSolver_Progress &progress);
double Calculate_Fitness(const double* solution, const size_t mProblem_Size);