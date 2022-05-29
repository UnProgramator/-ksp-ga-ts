#include <iostream>
#include "KSP_DS.h"
#include "Backtracking.h"
#include "NeighborhoodSearch.h"
#include "TabuSearch.h"

void test(KSP_DS& ds) {
	Backtracking bx(ds);
	bx.execute();
	std::cout << "N" + ds.N << ": " << bx.get_duration() << "\n";
}

int main() {
	init_from_file("DS.txt");
	Backtracking b8(ds8);
	Backtracking b10(ds10);
	Backtracking b50(ds50);
	Backtracking b100(ds100);

	/*ds8.print(std::cout);
	ds10.print(std::cout);
	ds50.print(std::cout);
	ds100.print(std::cout);*/

	b8.execute();
	b8.print_values(std::cout);
	std::cout << "N8: " << b8.get_duration() << "\n";

	/*b10.execute();
	b10.print_values(std::cout);
	std::cout << "N10: " << b10.get_duration() << std::endl;
	
	b50.execute();
	b50.print_values(std::cout);
	std::cout << "N50: " << b50.get_duration() << std::endl;
	
	b100.execute();
	b100.print_values(std::cout);
	std::cout << "N100: " << b100.get_duration() << std::endl;*/

	NeighborhoodSearch ns;

	std::cout << "\n\nNS:\n\n";

	/*for (int i = 0; i < 10; i++) {
		ns.benchmark(&ds8, std::cout);
	}*/

	

	for (int i = 0; i < 5; i++) {
		TabuSearch ts(ds8);
		auto sol = ts.execute();
		sol.print();

		std::cout << "--------------------\n\n";
	}
}