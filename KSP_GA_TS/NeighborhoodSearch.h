#pragma once
#include "KSP_DS.h"
#include <time.h>
#include <ostream>


class NeighborhoodSearch
{
private:
	const KSP_DS* set;

	unsigned int* solution = nullptr;

	KSP_DS::weight_type crtWeight = 0;

	void genStartingSolution();

	unsigned int getBestChange();

public:

	unsigned int* getSolution(const KSP_DS* dataSet);

	void benchmark(const KSP_DS* dataSet, std::ostream&);
};

