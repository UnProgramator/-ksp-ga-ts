#pragma once
#include "KSP_DS.h"
# include <tuple>
class SimulatedAnnealing
{
public:
	const KSP_DS& dataSet;
	SimulatedAnnealing() = delete;
	SimulatedAnnealing(const KSP_DS& dataSet, unsigned int initialTemp, unsigned int targetTemp, float coolingRation);

	unsigned* execute();

private:
	unsigned int crtTemp;
	const unsigned int targetTemp;
	const float coolingRation;

	struct { unsigned int* solution; int besfF; KSP_DS::weight_type g, v; } best, crt;
	unsigned int* neighbour;

	void genInitial();
	void getRandomNeighbour();

	std::tuple<int, KSP_DS::weight_type, KSP_DS::weight_type> f(const unsigned* const);
	unsigned alpha(unsigned);
};

