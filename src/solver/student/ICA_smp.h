#pragma once

#include"ICA.h"
#include<tbb/parallel_for.h>
#include<tbb/blocked_range.h>
#include<tbb/concurrent_vector.h>

#include "../../common/iface/SolverIface.h"
#include "Country.h"

#if defined(_MSC_VER) && defined(_Wp64)
// Workaround for overzealous compiler warnings in /Wp64 mode
#pragma warning (disable: 4267)
#endif /* _MSC_VER && _Wp64 */

class ICA_smp : public ICA {
private:
	//concurrency::concurrent_vector<Country> pop;
	//concurrency::concurrent_vector<Imperialist> imp;

public:
	ICA_smp(const solver::TSolver_Setup& setup);
	~ICA_smp() = default;

	virtual void gen_population() override;
	virtual void evolve() override;
	virtual void move_all_colonies(Imperialist& imp) override;
	virtual void migrate_colonies() override;

	virtual void calc_fitness_all() override;
	virtual double calc_fitness_imp(const Imperialist& imp) override;
};