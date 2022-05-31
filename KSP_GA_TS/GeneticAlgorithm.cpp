#include "GeneticAlgorithm.h"
#include <chrono>

GeneticAlgorithm::GeneticAlgorithm(const KSP_DS& dataSet, unsigned int populationSize):
	dataset{dataSet},
	L{dataSet.N},
	n{populationSize}
{
	P = new unsigned* [n];
	for (unsigned i = 0; i < n; i++)
		P[i] = new unsigned[L];
	Pp = new unsigned* [n];
	for (unsigned i = 0; i < n; i++)
		Pp[i] = new unsigned[L];
	best.best = new unsigned[L];
	fitness = new int[n];
}

GeneticAlgorithm::~GeneticAlgorithm() {
	for (unsigned i = 0; i < n; i++) {
		delete[] P[i];
		delete[] Pp[i];
	}
	delete[] P;
	delete[] Pp;
}

#define compy_toBest()\
memcpy(best.best, P[bestFittedIndex], L * sizeof(best.best[0]));\
best.fitnessOfTheBest = fitness[bestFittedIndex]


int* GeneticAlgorithm::execute(const int loops)
{
	initPopulation(); //initialize the population
	
	int bestFittedIndex = applyFitnessFunction();
	compy_toBest();
	
	int iteration = 0;

	while (iteration < loops || (getBestG() > dataset.G && iteration < loops + 100)) {
		oldFitness = newFiness;

		//get next get

		//reevaluate fitest
		bestFittedIndex = applyFitnessFunction();
		//if bettern than the best, then become the best
		if (best.fitnessOfTheBest < fitness[bestFittedIndex]) {
			compy_toBest();
		}
	}

	return nullptr;
}

std::pair<int*, double> GeneticAlgorithm::benchmark(int loops)
{
	auto start_timepoint = std::chrono::high_resolution_clock::now();

	auto sol = execute(loops);

	auto end_timepoint = std::chrono::high_resolution_clock::now();

	auto start_us = std::chrono::time_point_cast<std::chrono::nanoseconds>(start_timepoint).time_since_epoch().count();
	auto end_us = std::chrono::time_point_cast<std::chrono::nanoseconds>(end_timepoint).time_since_epoch().count();

	auto duration = (end_us - start_us) * 0.000001; // convert from nanosecond to millisecond, * 10^-6

	return std::pair<int*, double>(sol, duration);
}

void GeneticAlgorithm::initPopulation()
{
	for (unsigned i = 0; i < n; i++)
		for (unsigned j = 0; j < L; j++)
			P[i][j] = rand() & 1; // get if the last bite is 1
}

int GeneticAlgorithm::fitnessFunction(unsigned int* ent)
{
	KSP_DS::weight_type v =0 , g = 0;

	for (unsigned i = 0; i < L; i++) {
		if (ent[i] /*== 1*/) {
			v += dataset.v[i];
			g += dataset.g[i];
		}
	}

	int retVal = ((int)v) - ((int)g);

	if (g > dataset.G)
		retVal -= ((int)(g - dataset.G) << 1); //if we add to much weight, the fitness must be lower the fitnes with that ammout, taken twice
		//retVal -= ((int)g - dataset.G)<<2; // version 2 : four time the amount it overweights
	
	return retVal;
}

int GeneticAlgorithm::applyFitnessFunction()
{
	newFiness = 0;
	int bestFitted = 0;
	for (unsigned i = 0; i < n; i++) {
		fitness[i] = fitnessFunction(P[i]);
		newFiness += fitness[i];
		if (fitness[bestFitted] < fitness[i])
			bestFitted = i;
	}

	return bestFitted;
}

KSP_DS::weight_type GeneticAlgorithm::getBestG()
{
	KSP_DS::weight_type g = 0;
	for (unsigned index = 0; index < L; index++)
		if (best.best[index])
			g += dataset.g[index];
	return g;
}
