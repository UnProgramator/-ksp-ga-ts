#pragma once

#include "KSP_DS.h"
#include <list>

struct solution {
	unsigned int* solution_array;
	unsigned int solution_limit = -1;
	KSP_DS::weight_type weight = 0;
	KSP_DS::weight_type value = 0;
	size_t allocatedSize;

	solution& operator=(const solution&);
	solution& operator=(solution&&);

	solution(unsigned int size);
	solution() = delete;
	solution(const solution&);
	solution(solution&&);
	~solution();

	inline bool isBetterThan(solution other) const { return value > other.value || value == other.value && weight < other.weight; }

	void print();
};

class TabuSearch
{
public:
	TabuSearch(const KSP_DS&);
	TabuSearch() = delete;
	~TabuSearch() = default;

	solution execute(unsigned int max_iterations = 0, unsigned int tenure = 0);
	std::pair<solution, double> benchmark(unsigned int max_iterations, unsigned int tenure = 0);

	const KSP_DS& dataSet;

private:
	solution Sbest;
	solution S;
	solution Snext;
	
	std::list<std::pair<unsigned, unsigned>> tabus;
	unsigned tenure;

	void genStartSolution();
	void genNextSolution();
	bool check(const unsigned it, const unsigned i2);

	
};



