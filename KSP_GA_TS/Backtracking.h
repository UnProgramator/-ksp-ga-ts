#pragma once
#include "KSP_DS.h"

class Backtracking {
	KSP_DS& ksp_ds;

	long double duration = -1;

	uint64_t max_value = 0; // in principle we do not have items with value 0, all items have at least the value 1 (for price)
	uint64_t added_g = 0;
	int* items;
	unsigned int occupied;

public:

	Backtracking(KSP_DS&);
	Backtracking() = delete;
	~Backtracking();

	inline long double get_duration() { return duration; }

	void execute(); //the backtracking algorithm with time measurement
	inline void execute_notime() { backtrack_alg(); }

	void print_values(std::ostream& f);

private:

	void backtrack_alg();

	bool check(uint64_t, uint64_t, int*, unsigned int);
};


