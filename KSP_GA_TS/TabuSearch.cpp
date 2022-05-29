#include "TabuSearch.h"
#include <chrono>
#include <iostream>
#include <cstdlib>

struct move {
	unsigned src;
	unsigned dest;
	KSP_DS::weight_type new_weight = 0;
	KSP_DS::weight_type new_value = 0;
};

TabuSearch::TabuSearch(const KSP_DS& data_set): dataSet{ data_set }, Sbest(data_set.N), S(data_set.N), Snext(data_set.N)
{
}

solution TabuSearch::execute(unsigned int max_iterations, unsigned int tenure)
{
	if (tenure == 0)
		tenure = (unsigned)sqrt(dataSet.N)+1;
	if (max_iterations == 0)
		max_iterations = tenure + 10;
	this->tenure = tenure;
	genStartSolution();

	for (unsigned i = 0; i < max_iterations; i++) { //step
		genNextSolution(); //inspect the neighbourhood and choose the best alternative
	}

	return Sbest;
}

std::pair<solution,double> TabuSearch::benchmark(unsigned int max_iterations, unsigned int tenure)
{
	auto start_timepoint = std::chrono::high_resolution_clock::now();

	auto sol = execute(max_iterations);

	auto end_timepoint = std::chrono::high_resolution_clock::now();

	auto start_us = std::chrono::time_point_cast<std::chrono::nanoseconds>(start_timepoint).time_since_epoch().count();
	auto end_us = std::chrono::time_point_cast<std::chrono::nanoseconds>(end_timepoint).time_since_epoch().count();

	auto duration = (end_us - start_us) * 0.000001; // convert from nanosecond to millisecond, * 10^-6

	return std::pair<solution, double>(sol, duration);
}

void TabuSearch::genStartSolution()
{
	unsigned int i = 0;
	bool k = true;



	while (i < dataSet.N) {
		yesIditThis_genStartSolution:
		auto val = rand() % dataSet.N;

		for (unsigned int j = 0; j < i; j++)
			if (S.solution_array[j] == val)
				goto yesIditThis_genStartSolution;

		S.solution_array[i] = val;
		i++;
		if (k) {
			if (S.weight + dataSet.g[val] <= dataSet.G) {
				S.weight += dataSet.g[val];
				S.value += dataSet.v[val];
			}
			else { //the solution ends here
				k = false;
				S.solution_limit = i-1;
			}
		}
	}
	if (S.solution_limit == -1)
		S.solution_limit = i;
	Sbest = S;
	Snext = S;
	//S.print();
}

bool TabuSearch::check(const unsigned index1, const unsigned index2)
{
	//after the swap I take the new values
	auto isrc = S.solution_array[index1]; // the original values for dest
	auto idest = S.solution_array[index2]; // the original values for src

	//compute the weight and value for the neigbour of S where element from index i1 and i2 are swapped
	std::swap(S.solution_array[index1], S.solution_array[index2]);

	KSP_DS::weight_type weight = S.weight;
	KSP_DS::weight_type	value = S.value;
	if (isrc < S.solution_limit) {
		weight = weight - dataSet.g[isrc] + dataSet.g[idest]; //re-compute weight
		value = value - dataSet.v[isrc] + dataSet.v[idest]; //re-compute value
	}
	unsigned int k = S.solution_limit;

	if (weight < dataSet.G) {
		while (k < dataSet.N && weight + dataSet.g[k] < dataSet.G) { //see if we can add more elements from the "other" after the swap
			weight += dataSet.g[S.solution_array[k]];
			value += dataSet.v[S.solution_array[k]];
			k++;
		}
	}
	else if(weight > dataSet.G) {
		while (weight > dataSet.G && k > 0) {
			k--;
			weight -= dataSet.g[S.solution_array[k]];
			value -= dataSet.v[S.solution_array[k]];
		}
	}

	std::swap(S.solution_array[index1], S.solution_array[index2]); // remake the inital array

	auto pr = (isrc < idest ? std::pair<unsigned, unsigned>(isrc, idest) : std::pair<unsigned, unsigned>(idest, isrc));

	auto is = std::find(tabus.begin(), tabus.end(), pr);
	
	if (value < Snext.value || (value == Snext.value && weight < Snext.weight)) { //better solution for this iteration

		if (is == tabus.end() || //it is not a taboo move (is not in the taboo list)
			(value > Sbest.value || //or it is tabu but is better than the best solution so far => aspiration criterion
				(value == Sbest.value && weight < Snext.weight))) {

			Snext.value = value;
			Snext.weight = weight;
			Snext.solution_limit = k;
			//for now we do not consider to move the array for better performance
			return true;
		}

	}

	return false;
}


/*my idea of implementation is easy : check relevant neighbours :
 * solution: I divide the solution vector i 2 parts: [the solution | items which exists but exceds the weight]
 * relevant neighbour: a neighbour which can make a difference if inspected
 * irelevant neighbour: swapping 2 elements from the "solution part" or 2 elements from the "other parts, execept the first element from this part
 * => relevant neighbours:
 *		1. neigbours in which an element from the "solution" is swapped with one from the "others".
 *		2. neighbour where the first element from "others" is swapped with another element from there.
 * idea: 1'. when swapping 2 elements from "solution" you do not change the solution, we care of the total weight/value, 
 *			the order in this case doesn't make any difference
 *		 2'. when swapping 2 elements from the "others" part, except if one of the elements is the first, you do not change.
 * explanation: the "solution" are all elements which come one after the other until the weight limit is achieved.
*/
void TabuSearch::genNextSolution()
{
	Snext.value = -1; // invalidate the current "next";
	unsigned int p1, p2;
	for (unsigned int i = 0; i < S.solution_limit; i++) { // inspect if an element can be swap for better results.
		for (unsigned int j = S.solution_limit; j < S.allocatedSize; j++) {//try every posibility, inspect all relevant neighbours
			if (check(i, j)) {
				p1 = i;
				p2 = j;
			}
		}
	}

	for (unsigned int i = S.solution_limit + 1; i < dataSet.N; i++) { //try for the "other" part
		if (check(S.solution_limit, i)) {
			p1 = S.solution_limit;
			p2 = i;
		}
	}

	//better performance for S=Snext
	std::swap(S.solution_array[p1], S.solution_array[p2]); //before the iteration Snext and S are the same
	S.weight = Snext.weight;
	S.value = Snext.value;
	S.solution_limit = Snext.solution_limit;


	if (S.isBetterThan(Sbest))
		Sbest = S;

	//std::cout << p1 << " - " << p2 << "\n";

	//S.print();

	tabus.push_back(std::pair<unsigned, unsigned>(p1, p2));
	if (tabus.size() == tenure) //if overflow then remove oldest entry
		tabus.pop_front();
}


solution::solution(unsigned int size) {
	solution_array = new unsigned int[size];
	allocatedSize = size;
}

solution::solution(const solution& other) {
	solution_array = new unsigned int[other.allocatedSize];
	allocatedSize = other.allocatedSize;
	memcpy(solution_array, other.solution_array, allocatedSize*sizeof(solution_array[0]));
	solution_limit = other.solution_limit;
	weight = other.weight;
	value = other.value;
}

solution::solution(solution&& other) {
	solution_array = other.solution_array;
	allocatedSize = other.allocatedSize;
	other.solution_array = nullptr;
	solution_limit = other.solution_limit;
	weight = other.weight;
	value = other.value;
}

solution::~solution() {
	if (solution_array != nullptr)
		delete[] solution_array;
}

solution& solution::operator=(const solution& other) {
	if (solution_array != nullptr) {
		if (other.allocatedSize != allocatedSize) {
			delete[] solution_array;
			solution_array = new unsigned int[other.allocatedSize];
		}
	}
	else
		solution_array = new unsigned int[other.allocatedSize];
	allocatedSize = other.allocatedSize;
	memcpy(solution_array, other.solution_array, allocatedSize*sizeof(solution_array[0]));
	solution_limit = other.solution_limit;
	weight = other.weight;
	value = other.value;
	return *this;
}

solution& solution::operator=(solution&& other) {
	if (solution_array != nullptr)
		delete[] solution_array;
	allocatedSize = other.allocatedSize;
	solution_array = other.solution_array;
	other.solution_array = nullptr;
	solution_limit = other.solution_limit;
	weight = other.weight;
	value = other.value;
	return *this;
}

void solution::print() {
	std::cout << "w: " << weight << " ; v: " << value << " ; limit: " << solution_limit << "\n";
	for (unsigned i = 0; i < solution_limit; i++)
		std::cout << solution_array[i] << " ";
	std::cout << "| ";
	for(unsigned i = solution_limit; i< allocatedSize; i++)
		std::cout << solution_array[i] << " ";
	std::cout << "\n\n";
}