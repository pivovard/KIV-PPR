#include "..\solver_smp.h"
#include<list>
#include<vector>
#include <iostream>
#include<random>
#include "ImperialistAlg.h"

//using TObjective_Function = double(IfaceCalling*)(const void* data, const double* solution);

HRESULT solve_smp(solver::TSolver_Setup &setup, solver::TSolver_Progress &progress) {
	
	Country col;
	col.vec = std::vector<double>{ 1,1 };
	Country im;
	im.vec = std::vector<double>{ 2,2 };

	


	ImperialistAlg imp(setup);
	imp.move_colony(im, col);
	std::cout << col.vec[0] << " " << col.vec[1] << std::endl;
	system("pause");

	imp.gen_population();
	
	imp.evolve();
	imp.print_population();

	imp.write_solution();


	system("pause");
	return S_OK;
}