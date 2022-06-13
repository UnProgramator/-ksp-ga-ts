#include <iostream>
#include <cstdlib>
#include <ctime>
#include "KSP_DS.h"
#include "Backtracking.h"
#include "NeighborhoodSearch.h"
#include "TabuSearch.h"
#include "GeneticAlgorithm.h"
#include "SimulatedAnnealing.h"

void test(KSP_DS& ds) {
	Backtracking bx(ds);
	bx.execute();
	std::cout << "N" + ds.N << ": " << bx.get_duration() << "\n";
}

void bk() {
	Backtracking b8(ds8);
	Backtracking b10(ds10);
	Backtracking b50(ds50);
	//Backtracking b100(ds100); // no use, can't comput in "realistic" time

	b8.execute();
	b8.print_values(std::cout);
	std::cout << "N8: " << b8.get_duration() << "ms\n";

	b10.execute();
	b10.print_values(std::cout);
	std::cout << "N10: " << b10.get_duration() << "ms\n";
	
	b50.execute();
	b50.print_values(std::cout);
	std::cout << "N50: " << b50.get_duration() << "ms\n";
	
	/*b100.execute();
	b100.print_values(std::cout);
	std::cout << "N100: " << b100.get_duration() << "ms\n";*/

}

void run(KSP_DS& ds, int iters = 2) {
	static NeighborhoodSearch ns;
	for (int i = 0; i < iters; i++) {
		ns.benchmark(&ds, std::cout);
	}
}

void ns() {
	run(ds8);
	run(ds10);
	run(ds50);
	run(ds100);
}

void executeTs(std::string name, TabuSearch tsi, std::initializer_list<unsigned int> tenures, int repeate_time = 10) {
	std::cout << " ------------" << name << ":------------\n\n";

	solution Sbest(tsi.dataSet.N);

	double totalTime;

	for (auto tenure : tenures) {
		double time = 0;
		totalTime = 0;
		Sbest.value = 0;
		std::cout << "tenure " << tenure << "\n";
		for (int i = 0; i < 10; i++) {
			auto result = tsi.benchmark(repeate_time, tenure);
			if (result.first.value > Sbest.value
				|| result.first.value == Sbest.value && result.first.weight < Sbest.weight) {
				Sbest = result.first;
				time = result.second;
			}
			totalTime += result.second;
		}
		Sbest.print();

		std::cout << "best result time: " << time << "\n with a total of " << totalTime / 10  << "\n\n";
	}

	
}

void ts() {
	executeTs("DS8", TabuSearch(ds8), {1, 3, 7}, 1000);
	executeTs("DS10", TabuSearch(ds10), {1, 6, 9}, 1000);
	executeTs("DS50", TabuSearch(ds50), {1, 14, 48}, 1000);
	executeTs("DS100", TabuSearch(ds100), {1, 60, 120}, 1000);
}

void runGa(GeneticAlgorithm ga, int iterRate, const KSP_DS& ds) {

	auto result = ga.benchmark(iterRate);

	KSP_DS::weight_type g = 0, v = 0;

	for (unsigned i = 0; i < ds.N; i++) {
		if (result.first[i]) {
			std::cout << i << " ";
			g += ds.g[i];
			v += ds.v[i];
		}
	}
	std::cout << "\ng: " << g << " / " << ds.G << " v = " << v;
	std::cout << "\ntime: " << result.second << "ms\n\n";
	delete[] result.first;
}

void ga() {
	std::cout << "-----------DS8----------\n\n";
	runGa(GeneticAlgorithm(ds8, 800, 85, 10), 500, ds8);
	runGa(GeneticAlgorithm(ds8, 1000, 60, 22), 1000, ds8);
	runGa(GeneticAlgorithm(ds8, 1000, 85, 10), 1000, ds8);

	std::cout << "-----------DS10---------\n\n";
	runGa(GeneticAlgorithm(ds10, 800, 85, 10), 500, ds10);
	runGa(GeneticAlgorithm(ds10, 1000, 60, 22), 1000, ds10);
	runGa(GeneticAlgorithm(ds10, 1000, 85, 10), 1000, ds10);

	std::cout << "-----------DS50---------\n\n";
	runGa(GeneticAlgorithm(ds50, 800, 85, 10), 500, ds50);
	runGa(GeneticAlgorithm(ds50, 1000, 60, 22), 1000, ds50);
	runGa(GeneticAlgorithm(ds50, 1000, 85, 10), 1000, ds50);

	std::cout << "-----------DS100---------\n\n";
	runGa(GeneticAlgorithm(ds100, 800, 85, 10), 500, ds100);
	runGa(GeneticAlgorithm(ds100, 1000, 60, 22), 1000, ds100);
	runGa(GeneticAlgorithm(ds100, 1000, 85, 10), 1000, ds100);
}

void runSA(SimulatedAnnealing sa) {
	auto* result = sa.execute();
	KSP_DS::weight_type g = 0, v = 0;

	for (unsigned i = 0; i < sa.dataSet.N; i++) {
		if (result[i]) {
			std::cout << i << " ";
			v += sa.dataSet.v[i];
			g += sa.dataSet.g[i];
		}
	}

	std::cout << "\ng = " << g << "/" << sa.dataSet.G << ", v = " << v << "\n\n";
}

void sa() {
	std::cout << "---------------DS8---------------\n";
	runSA(SimulatedAnnealing(ds8, 1000, 200, 0.9f));
	runSA(SimulatedAnnealing(ds8, 1200, 200, 0.9f));
	runSA(SimulatedAnnealing(ds8, 1800, 700, 0.85f));

	std::cout << "---------------DS10--------------\n";
	runSA(SimulatedAnnealing(ds10, 1000, 200, 0.9f));
	runSA(SimulatedAnnealing(ds10, 1200, 200, 0.9f));
	runSA(SimulatedAnnealing(ds10, 1800, 700, 0.85f));

	std::cout << "---------------DS50--------------\n";
	runSA(SimulatedAnnealing(ds50, 1000, 200, 0.9f));
	runSA(SimulatedAnnealing(ds50, 1200, 200, 0.9f));
	runSA(SimulatedAnnealing(ds50, 1800, 700, 0.85f));

	std::cout << "--------------DS100--------------\n";
	runSA(SimulatedAnnealing(ds100, 1000, 200, 0.9f));
	runSA(SimulatedAnnealing(ds100, 1200, 200, 0.9f));
	runSA(SimulatedAnnealing(ds100, 1800, 700, 0.85f));
}

int main() {
	//init random engine
	srand((unsigned)time(NULL));

	init_from_file("DS.txt");
	
	//print dataset on screen
	//ds8.print(std::cout);
	//ds10.print(std::cout);
	//ds50.print(std::cout);
	//ds100.print(std::cout);
	
	//bk();

	//ns();

	//ga();
	
	//ts();

	sa();

	return 0;
}