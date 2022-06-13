#include "NeighborhoodSearch.h"
#include <cstdlib>
#include <chrono>

void NeighborhoodSearch::genStartingSolution()
{

	solution = new unsigned int[set->N];

	for (unsigned int i = 0; i < set->N; i++)
		solution[i] = 0;

	while (1) {
		int index = rand()%set->N;

		if (set->g[index] > set->G)
			continue;
		solution[index] = 1;
		crtWeight = set->g[index];
		return;
	}
}

//generate the position that need to be flipped to generate the new solution, if any new s' is available
//return -1 if there is no new better neigbour
unsigned int NeighborhoodSearch::getBestChange()
{
	int next_val = -1;

	for (unsigned int i = 0; i < set->N; i++)
		if (solution[i] == 0) //if neighbour is null
			if(set->g[i] + crtWeight <= set->G) //if this could choice could result in a valid neighbour solution
				if (next_val == -1) //is the first consideration
					next_val = i; //set this value for the "neigbour" (crt solution where i is 1 is the neighbour)
				else if (set->v[i] > set->v[next_val]) // if this neighbour can generate a heigher value
					next_val = i; //set this value for the "neigbour" (crt solution where i is 1 is the neighbour)
				else if(set->v[i] == set->v[next_val] && set->g[i] < set->g[next_val]) //if has the same value but weights less
					next_val = i; //set this value for the "neigbour" (crt solution where i is 1 is the neighbour)

	return next_val;
}

unsigned int* NeighborhoodSearch::getSolution(const KSP_DS* dataSet)
{
	this->set = dataSet;
	
	genStartingSolution();

	int index = -1;

	while(true) {
		index = getBestChange(); //get s' ; in this case, we get what position need to be updated in order to get s' from s

		if (index == -1) //if the position is -1 then there is no better s' than s and we finish the func
			break;

		solution[index] = 1; //this implemented the s = s'
		crtWeight += set->g[index];
	}

	this->set = nullptr; //"cleanup"
	return solution;
}

void NeighborhoodSearch::benchmark(const KSP_DS* dataSet, std::ostream& outStream)
{
	auto start_timepoint = std::chrono::high_resolution_clock::now();

	auto sol = getSolution(dataSet);

	auto end_timepoint = std::chrono::high_resolution_clock::now();

	auto start_us = std::chrono::time_point_cast<std::chrono::nanoseconds>(start_timepoint).time_since_epoch().count();
	auto end_us = std::chrono::time_point_cast<std::chrono::nanoseconds>(end_timepoint).time_since_epoch().count();

	auto duration = (end_us - start_us) * 0.000001; // convert from nanosecond to millisecond, * 10^-6

	outStream << "DS" << dataSet->N << "\nDuration: " << duration << "\n";
	
	KSP_DS::weight_type weight = 0, value = 0;
	for (unsigned int i = 0; i < dataSet->N; i++) {
		if (sol[i]) {
			outStream << i << " ";
			weight += dataSet->g[i];
			value += dataSet->v[i];
		}
	}
	outStream << "\nWith weight=" << weight << " and value=" << value << "\n\n";
}

