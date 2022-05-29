#include "Backtracking.h"

#include <chrono>
#include <cstdint>
#include <ostream>
#include <iostream>

  
Backtracking::Backtracking(KSP_DS& param) :ksp_ds{param} {
	items = new int[param.N];
}

Backtracking::~Backtracking() {

}

void Backtracking::execute(){
	auto start_timepoint = std::chrono::high_resolution_clock::now();

	backtrack_alg();

	auto end_timepoint = std::chrono::high_resolution_clock::now();

	auto start_us = std::chrono::time_point_cast<std::chrono::nanoseconds>(start_timepoint).time_since_epoch().count();
	auto end_us = std::chrono::time_point_cast<std::chrono::nanoseconds>(end_timepoint).time_since_epoch().count();

	duration = (end_us - start_us) * 0.000001; // convert from nanosecond to millisecond, * 10^-6
}

void Backtracking::print_values(std::ostream& f)
{
	for (unsigned int i = 0; i < occupied; i++) {
		f << items[i] << " ";
	}
	f << "\n" << "Value = " << max_value << "\n";
	f.flush();
}

void Backtracking::backtrack_alg(){
	unsigned int i;
	int *index = new int[this->ksp_ds.N];
	uint64_t g_sum = 0;
	uint64_t v_sum = 0;

	index[0] = -1;
	i = 0;
	while (true) {
		if (index[i] < ((int)ksp_ds.N) - 1) { //update the curent index
			index[i]++;
		}
		else {
			if (i == 0) //exit condition
				break;
			i--;
			g_sum -= ksp_ds.g[index[i]]; //we will change the previouse item, so we eliminate the crt item
			v_sum -= ksp_ds.v[index[i]]; //we will change the previouse item, so we eliminate the crt item
			continue; //go one position lower on the stack, so next iteration
		}

		if (g_sum + ksp_ds.g[index[i]] <= ksp_ds.G) { // we can still add items to bag
			g_sum += ksp_ds.g[index[i]]; //we add the current item
			v_sum += ksp_ds.v[index[i]]; //we add the value to the total value
			if (i < ksp_ds.N -1) { //we test if we still have items to add
				i++;
				index[i] = index[i-1];
				continue; // we add the item and see if we can still add more in the next iteration
			}
			else {
				check(v_sum, g_sum, index, i); //we do not have any more items, so we check if we have a hit
				g_sum -= ksp_ds.g[index[i]]; //we will change the crt item, so we eliminate the crt it from the bag
				v_sum -= ksp_ds.v[index[i]]; //we will change the crt item, so we eliminate the crt it from the bag
			}
		}
		else {//we cannot add any more
			check(v_sum,g_sum, index, i); //couldn't add the i'th item so it's not counted, so i-1
		}
		
	}
}

//#define on_DEBUG_bk

bool Backtracking::check(uint64_t added_v, uint64_t total_g, int* combination, unsigned int array_size)
{
#ifdef on_DEBUG_bk
	std::cout << added_v << " g=" << total_g << "\n";
	/*for (int i = 0; i < array_size; i++)
		std::cout << combination[i] << " ";
	std::cout << "\n";*/
#endif // on_DEBUG_bk


	if (added_v > this->max_value) {
		max_value = added_v;
		memcpy(items, combination, array_size * sizeof(unsigned int));
		occupied = array_size;
	}
	return false;
}