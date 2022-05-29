#include "KSP_DS.h"

#include <exception>
#include <istream>
#include <fstream>
#include <iostream>
#include <random>
#include <functional>

KSP_DS ds8(8);
KSP_DS ds10(10);
KSP_DS ds50(50);
KSP_DS ds100(100);


KSP_DS::KSP_DS(unsigned int N, weight_type G) : N{N}, G{G} {
	g = new weight_type[N];
	v = new weight_type[N];

	if (g == nullptr || v == nullptr) {
		throw std::exception("memory not available");
	}
}

KSP_DS::~KSP_DS()
{
	if (g) delete g;
	if (v) delete v;
}

void KSP_DS::read_data(std::istream& in)
{
	for (unsigned int i = 0; i < N; i++) {
		in >> g[i];
	}

	for (unsigned int i = 0; i < N; i++) {
		in >> v[i];
	}
}

void KSP_DS::print(std::ostream& os)
{
	os << "N" << N << "\nG " << G << "\n";
	for (unsigned int i = 0; i < N; i++)
		os << g[i] << " ";
	os << "\n";
	for (unsigned int i = 0; i < N; i++)
		os << v[i] << " ";
	os << "\n\n";
	os.flush();
}

void init_from_file(const char* src)
{
	std::ifstream f(src);
	
	//#N8
	f.ignore(10, '\n');
	f >> ds8.G;

	for (unsigned int i = 0; i < ds8.N; i++)
		f >> ds8.g[i];

	for (unsigned int i = 0; i < ds8.N; i++)
		f >> ds8.v[i];

	//#N10
	f.get(); //skip the blank line
	f.get(); //skip the blank line
	f.ignore(10, '\n');

	f >> ds10.G;

	for (unsigned int i = 0; i < ds10.N; i++)
		f >> ds10.g[i];

	for (unsigned int i = 0; i < ds10.N; i++)
		f >> ds10.v[i];

	//#N50
	f.get(); //skip the blank line
	f.get(); //skip the blank line
	f.ignore(10, '\n');
	f >> ds50.G;

	for (unsigned int i = 0; i < ds50.N; i++)
		f >> ds50.g[i];

	for (unsigned int i = 0; i < ds50.N; i++)
		f >> ds50.v[i];

	//#N100
	f.get(); //skip the blank line
	f.get(); //skip the blank line
	f.ignore(10, '\n');
	f >> ds100.G;

	for (unsigned int i = 0; i < ds100.N; i++)
		f >> ds100.g[i];

	for (unsigned int i = 0; i < ds100.N; i++)
		f >> ds100.v[i];
}

void init_from_random() {
	ds8.G = (KSP_DS::weight_type) 0.62 * rand_gen(8, 1, 16, ds8.g);
	rand_gen(8, 20, 56, ds8.v);

	ds10.G = (KSP_DS::weight_type) 0.62 * rand_gen(10, 1, 20, ds10.g);
	rand_gen(10, 32, 76, ds10.v);

	ds50.G = (KSP_DS::weight_type) 0.62 * rand_gen(50, 2, 32, ds50.g);
	rand_gen(50, 10, 50, ds50.v);

	ds100.G = (KSP_DS::weight_type) 0.62 * rand_gen(100, 1, 50, ds100.g);
	rand_gen(100, 5, 78, ds100.v);
}

uint64_t rand_gen(unsigned int N, int mn, int mx, KSP_DS::weight_type* dest)
{
	uint64_t sum = 0;
	auto gen = std::bind(std::uniform_int_distribution<KSP_DS::weight_type>(mn, mx), std::default_random_engine());

	for (unsigned int i = 0; i < N; i++) {
		KSP_DS::weight_type next_num = gen();
		sum += next_num;
		dest[i] = next_num;
	}

	return sum;
}

#define write_g(N, mn, mx) \
sum_of_gen = rand_gen(N, mn, mx, temp);\
f << (uint64_t)(sum_of_gen * 0.62) << "\n"; \
for (unsigned int i = 0; i < N; i++) {\
	f << temp[i] << " ";\
}

#define write_v(N, mn, mx) \
sum_of_gen = rand_gen(N, mn, mx, temp);\
for (unsigned int i = 0; i < N; i++) {\
	f << temp[i] << " ";\
}

void gen_file(const char* dest)
{

	std::ostream &f= std::cout /**/;
	KSP_DS::weight_type temp[100];
	uint64_t sum_of_gen;

	f << "#8\n";
	write_g(8, 1, 12);//gen the weights
	f << std::endl;
	write_v(8, 10, 30); // gen the values
	f << "\n" << std::endl;
	
	f << "#10\n";
	write_g(10, 1, 10);//gen the weights
	f << std::endl;
	write_v(10, 1, 14); // gen the values
	f << "\n" << std::endl;

	f << "#50\n";
	write_g(50, 2, 32);//gen the weights
	f << std::endl;
	write_v(50, 10, 50); // gen the values
	f << "\n" << std::endl;

	f << "#100\n";
	write_g(100, 10, 45);//gen the weights
	f << std::endl;
	write_v(100, 13, 58); // gen the values
	f << "\n" << std::endl;

	//f.close();
}
