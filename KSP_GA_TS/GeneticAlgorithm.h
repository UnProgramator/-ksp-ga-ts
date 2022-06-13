#pragma once
#include "KSP_DS.h"


class GeneticAlgorithm
{
public:
	GeneticAlgorithm(const KSP_DS& dataSet, unsigned int populationSize, int crossoverProbabilty, int mutationProbabilty);
	GeneticAlgorithm() = delete;
	~GeneticAlgorithm();

	std::pair<unsigned*, double> benchmark(int loops = 0);
	unsigned* execute(int loops = 0);

private:
	void initPopulation();
	void selection();
	void process();
	void mutation(unsigned* individual);

	int fitnessFunction(unsigned int*);

	int applyFitnessFunction();

	KSP_DS::weight_type getBestG();

	inline bool tossTheCoin(int probability) {
		return (rand() % 100) < probability;
	}

	inline int getRandomInRange(int mn, int mx) {
		return rand() % (mx - mn) + mn;
	}

	inline int getRandomInRange(int mx) {
		return rand() % mx;
	}

	unsigned int** P;
	unsigned int** Pp;
	int* fitness;
	const unsigned int L;
	const unsigned int n;
	const KSP_DS& dataset;

	const int crossoverProbabilty;
	const int mutationProbabilty;

	int generationFitness = 0, oldFitness = 0;

	struct { unsigned* best = nullptr; int fitnessOfTheBest =0; } best;
};

