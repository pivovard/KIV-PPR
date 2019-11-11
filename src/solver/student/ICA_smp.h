#pragma once

#include"ICA.h"
#include<tbb/parallel_for.h>
#include<tbb/blocked_range.h>
#include<concurrent_vector.h>

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

	//virtual void print_population() override;
};