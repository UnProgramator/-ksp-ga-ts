#include "GeneticAlgorithm.h"
#include <chrono>
#include <iostream>
#include <ctime>

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
best.fitnessOfTheBest = fitnessFunction(P[bestFittedIndex]);
//std::cout<< best.fitnessOfTheBest << " -> ";\
//for(int i=0; i<L; i++)\
//	std::cout << best.best[i];\
//std::cout << "\n"



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

	auto start = std::clock();

	while (iteration < loops 
			|| (getBestG() > dataset.G && (std::clock() - start < CLOCKS_PER_SEC * 120)) //30 seconds timeout
		) {
		//get next get
		selection();

		process();

		std::swap(P, Pp); // we swap them so we do not have to copy them again, for performance; the only advantage to have * :))

		//reevaluate fitness
		bestFittedIndex = applyFitnessFunction();

		//if bettern than the best, then become the best
		if (best.fitnessOfTheBest < fitnessFunction(P[bestFittedIndex])) { //if the new fittest is fitter than the fittest from the current generation, then it changes, else no
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
			P[i][j] = rand() & 1; // get if the last bite is 1 50% chance
}

void GeneticAlgorithm::selection()
{
	//std::cout << generationFitness << " --- ";
	//Pp ~ S'
	for (unsigned int i = 0; i < n; i++) {
		auto result = getRandomInRange(generationFitness);

		unsigned int j = 0;
		while (result - fitness[j] > 0) result -= fitness[j++]; //turn the rullete; j will hold the result
																//if j will result the last, result - fitness[n-1] will == 0
		if (j >= n) j = n - 1;
		memcpy(Pp[i], P[j], L * sizeof(Pp[0][0]));
	}
}

void GeneticAlgorithm::process()
{
	//Pp ~ S' and P ~ S''
	int* temp = new int [dataset.N];
	unsigned index = n+1;
	for (unsigned int i = 0; i < n; i++) {

		auto probability = getRandomInRange(100);

		if (probability < crossoverProbabilty) { //choose crosover for crt individual
			if (index == n + 1) {// first individual chosen for crossover
				index = i; 
			}
			else { // second individual chosen for crossover => do crossover
				int point = getRandomInRange(2, L - 3); // minimum 2 at left or write, so it doesn't swapp
				memcpy(temp, Pp[i] + point, (L - point) * sizeof(Pp[0][0]));
				memcpy(Pp[i] + point, Pp[index] + point, (L - point) * sizeof(Pp[0][0]));
				memcpy(Pp[index] + point, temp, (L - point) * sizeof(Pp[0][0]));

				//give the chance to also get a mutation after crossover
				/*if (tossTheCoin(mutationProbabilty)) {
					mutation(Pp[i]);
				}
				if (tossTheCoin(mutationProbabilty)) {
					mutation(Pp[index]);
				}*/
				index = n + 1;
			}
		}
		else if (probability - crossoverProbabilty < mutationProbabilty) { //else if we choose mutation
			mutation(Pp[i]);
		}
		//esle is copy
	}
}

void GeneticAlgorithm::mutation(unsigned* ind)
{
	int probability = mutationProbabilty / 2; // first value
	for (unsigned int j = 0; j < L; j++) {
		if (tossTheCoin(mutationProbabilty)) {
			ind[j] = ind[j] ? 0 : 1;
			probability = mutationProbabilty / 2; //each time a mutation occures, I reset the probability
		}
		else {
			probability += mutationProbabilty / 2; //to ensure that at least one value is mutated, each time a mutation does not occure, I make the probability bigger
		}
	}
}

int GeneticAlgorithm::fitnessFunction(unsigned int* ent)
{
	KSP_DS::weight_type v =0 , g = 0;

	for (unsigned i = 0; i < L; i++) {
		if (ent[i]) {
			v += dataset.v[i];
			g += dataset.g[i];
		}
	}

	int retVal;

	if (g > dataset.G)
		retVal = ((int)v) - ((int)g);
	else
		retVal = int(((int)v * v) - ((int)g) - ((int)dataset.G - g));

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
		generationFitness += fitness[i];
	}

	if (min < 0) {
		min = -min + 1;

		for (unsigned i = 0; i < n; i++) { //move the fitnes form [-inf, inf] to [1, inf]; the negative fitness could result in problems, as well as 0
			fitness[i] += min;
		}
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
