#pragma once
#include "KSP_DS.h"


class GeneticAlgorithm
{
public:
	GeneticAlgorithm(const KSP_DS& dataSet, unsigned int populationSize);
	GeneticAlgorithm() = delete;
	~GeneticAlgorithm();

	int* execute(int loops = 0);
	std::pair<int*, double> benchmark(int loops = 0);

private:
	unsigned int** P;
	unsigned int** Pp;
	int* fitness;
	const unsigned int& L;
	const unsigned int n;
	const KSP_DS& dataset;

	int newFiness = 0, oldFitness = 0;

	struct { unsigned* best; int fitnessOfTheBest; } best;

	void initPopulation();
	int fitnessFunction(unsigned int*);

	int applyFitnessFunction();
	inline bool tossTheCoin(int probability) {
		return (rand() % 100) < probability;
	}
	KSP_DS::weight_type getBestG();
};

