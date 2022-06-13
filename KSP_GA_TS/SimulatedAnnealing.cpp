#include "SimulatedAnnealing.h"


SimulatedAnnealing::SimulatedAnnealing(const KSP_DS& dataSet, unsigned int initialTemp, unsigned int targetTemp, float coolingRation) :
	coolingRation(coolingRation),
	targetTemp(targetTemp),
	crtTemp(initialTemp),
	dataSet(dataSet),
	best{ nullptr, 0 },
	crt{new unsigned[dataSet.N], -1},
	neighbour(new unsigned[dataSet.N])
{
	if (coolingRation >= 1 || coolingRation <= 0)
		throw "not a valid cooling ration";
	if (crt.solution == nullptr || neighbour == nullptr)
		throw "could not initialize";
}

unsigned* SimulatedAnnealing::execute()
{
	genInitial();
	
	do {
		getRandomNeighbour();

		int d;
		KSP_DS::weight_type g, v;

		std::tie(d, v, g) = f(neighbour);

		if (d < crt.besfF) {
			memcpy(crt.solution, neighbour, dataSet.N * sizeof(neighbour[0]));
			crt.besfF = d;
			crt.g = g;
			crt.v = v;
			//if it's better than the best
			if (g <= dataSet.G &&(
					v >= best.v ||
					v == best.v && g < best.g)) {
				memcpy(best.solution, neighbour, dataSet.N * sizeof(neighbour[0]));
				best.besfF = d;
				best.g = g;
				best.v = v;
			}
		}
		else {
			double x = int(rand() % 100) / 100.0;
			if (x < exp(-double(d) / crtTemp)) {
				memcpy(crt.solution, neighbour, dataSet.N * sizeof(neighbour[0]));
				crt.besfF = d;
				crt.g = g;
				crt.v = v;
			}
		}

		crtTemp = alpha(crtTemp);
	} while (crtTemp > targetTemp);


	return best.solution;
}

void SimulatedAnnealing::genInitial()
{
	KSP_DS::weight_type g = 0;
	for (unsigned i = 0; i < dataSet.N; i++) {
		crt.solution[i] = 0;
	}

	while(true){
		unsigned poz = (unsigned) rand() % dataSet.N;
		if (crt.solution[poz] == 1) //if already set
			continue;
		if (g + dataSet.g[poz] > dataSet.G) //if too heavy
			break;
		crt.solution[poz] = 1; //init crt sol
		g += dataSet.g[poz];
	}

	best.solution = new unsigned[dataSet.N];
	memcpy(best.solution, crt.solution, dataSet.N * sizeof(crt.solution[0])); //initialy are the same
	auto fs = f(crt.solution);
	best.besfF = crt.besfF = std::get<0>(fs);
	best.g = crt.g = std::get<2>(fs);
	best.v = crt.v = std::get<1>(fs);
}

void SimulatedAnnealing::getRandomNeighbour()
{
	int poz = rand() % dataSet.N;
	memcpy(neighbour, crt.solution, dataSet.N * sizeof(neighbour[0]));
	neighbour[poz] = ~neighbour[poz];
}

std::tuple<int, KSP_DS::weight_type, KSP_DS::weight_type>  SimulatedAnnealing::f(const unsigned* const sol)
{
	KSP_DS::weight_type g = 0, v = 0;

	for (unsigned i = 0; i < dataSet.N; i++) {
		if (sol[i]) {
			g += dataSet.g[i];
			v += dataSet.v[i];
		}
	}
	if (g > dataSet.G) {
		return std::tuple<int, KSP_DS::weight_type, KSP_DS::weight_type>(int(v) - int(g) - int(g - dataSet.G), v, g);
	}
	else {
		return std::tuple<int, KSP_DS::weight_type, KSP_DS::weight_type>(int(v * v + (dataSet.G - g)), v, g);
	}
}

unsigned SimulatedAnnealing::alpha(unsigned)
{
	return (unsigned)(crtTemp * coolingRation);
}

