#include "GeneticAlgorithm.h"
#include <chrono>
#include <iostream>

GeneticAlgorithm::GeneticAlgorithm(const KSP_DS& dataSet, unsigned int populationSize, int crossoverProbabilty, int mutationProbabilty):
	dataset{dataSet},
	L{dataSet.N},
	n{populationSize},
	crossoverProbabilty{ crossoverProbabilty },
	mutationProbabilty{ mutationProbabilty }
{
	P = new unsigned* [n];
	for (unsigned i = 0; i < n; i++)
		P[i] = new unsigned[L];
	Pp = new unsigned* [n];
	for (unsigned i = 0; i < n; i++)
		Pp[i] = new unsigned[L];
	fitness = new int[n];
}

GeneticAlgorithm::~GeneticAlgorithm() {
	/*for (unsigned i = 0; i < n; i++) {
		delete[] P[i];
		delete[] Pp[i];
	}
	delete[] P;
	delete[] Pp;
	delete[] fitness;*/
}

#define compy_toBest()\
memcpy(best.best, P[bestFittedIndex], L * sizeof(best.best[0]));\
best.fitnessOfTheBest = fitness[bestFittedIndex]

std::pair<unsigned*, double> GeneticAlgorithm::benchmark(int loops)
{
	auto start_timepoint = std::chrono::high_resolution_clock::now();

	auto sol = execute(loops);

	auto end_timepoint = std::chrono::high_resolution_clock::now();

	auto start_us = std::chrono::time_point_cast<std::chrono::nanoseconds>(start_timepoint).time_since_epoch().count();
	auto end_us = std::chrono::time_point_cast<std::chrono::nanoseconds>(end_timepoint).time_since_epoch().count();

	auto duration = (end_us - start_us) * 0.000001; // convert from nanosecond to millisecond, * 10^-6

	return std::pair<unsigned*, double>(sol, duration);
}

unsigned* GeneticAlgorithm::execute(const int loops)
{
	best.best = new unsigned[L]; //we return this value, so we init it every time
	initPopulation(); //initialize the population

	int bestFittedIndex = applyFitnessFunction();
	compy_toBest();

	int iteration = 0;

	while (iteration < loops || (getBestG() > dataset.G && iteration < loops + 100)) {
		//get next get
		selection();

		recombination();

		mutation();

		std::swap(P, Pp); // we swap them so we do not have to copy them again, for performance; the only advantage to have * :))

		//reevaluate fitness
		bestFittedIndex = applyFitnessFunction();

		//if bettern than the best, then become the best
		if (best.fitnessOfTheBest < fitness[bestFittedIndex]) {
			compy_toBest();
		}
		iteration++;
	}

	return best.best;
}

void GeneticAlgorithm::initPopulation()
{
	for (unsigned i = 0; i < n; i++)
		for (unsigned j = 0; j < L; j++)
			P[i][j] = rand() & 1; // get if the last bite is 1
}

void GeneticAlgorithm::selection()
{
	std::cout << generationFitness << " --- ";
	//Pp ~ S'
	for (unsigned int i = 0; i < n; i++) {
		auto result = getRandomInRange(generationFitness);

		int j = 0;
		while (result - fitness[j] > 0) result -= fitness[j++]; //turn the rullete; j will hold the result
																//if j will result the last, result - fitness[n-1] will == 0
		if (j >= n) j = n - 1;
		memcpy(Pp[i], P[j], L * sizeof(Pp[0][0]));
	}
}

void GeneticAlgorithm::recombination()
{
	//Pp ~ S' and P ~ S''
	int* temp = new int [dataset.N];
	for (unsigned int i = 0; i < n/2; i++) {
		if (tossTheCoin(crossoverProbabilty)) { // do a crossover
			int point = getRandomInRange(2, n - 3); // minimum 2 at left or write, so it doesn't swapp
			memcpy(temp, Pp[2 * i] + point, (L - point) * sizeof(Pp[0][0]));
			memcpy(Pp[2 * i] + point, Pp[2 * i] + point + 1, (L - point) * sizeof(Pp[0][0]));
			memcpy(Pp[2 * i] + point + 1, temp, (L - point) * sizeof(Pp[0][0]));
		}
	}
}

void GeneticAlgorithm::mutation()
{
	for (unsigned int i = 0; i < n ; i++) {
		for (unsigned int j = 0; j < L; j++) {
			if (tossTheCoin(mutationProbabilty))
				Pp[i][j] = Pp[i][j] ? 0 : 1;
		}
	}
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
	oldFitness = generationFitness;
	generationFitness = 0;
	int bestFitted = 0;
	int min = 0;
	for (unsigned i = 0; i < n; i++) {
		fitness[i] = fitnessFunction(P[i]);
		if (fitness[i] < 0 && fitness[i] < min) {
			min = fitness[i];
		}
		if (fitness[bestFitted] < fitness[i])
			bestFitted = i;
	}

	min = -min;

	for (unsigned i = 0; i < n; i++) { //move the fitnes form [-inf, inf] to [1, inf]; the negative fitness could result in problems, as well as 0
		fitness[i] += min+1;
		generationFitness += fitness[i];
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
