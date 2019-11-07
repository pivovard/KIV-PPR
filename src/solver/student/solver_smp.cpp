#include "..\solver_smp.h"
#include<list>
#include<vector>
#include <iostream>
#include<random>
#include "ImperialistAlg.h"

//using TObjective_Function = double(IfaceCalling*)(const void* data, const double* solution);

HRESULT solve_smp(solver::TSolver_Setup &setup, solver::TSolver_Progress &progress) {

	ImperialistAlg imp(setup);

	imp.gen_population();

	imp.evolve();
	imp.print_population();

	imp.write_solution();


	system("pause");
	return S_OK;
}