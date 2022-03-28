#pragma once
#include <string>

class KSP_DS
{
public:
	typedef uint64_t weight_type;

	weight_type G;
	const unsigned int N;
	weight_type* g;
	weight_type* v;

	KSP_DS(unsigned int N, weight_type G);
	KSP_DS(unsigned int N) :KSP_DS(N, 0) {}
	KSP_DS() = delete;
	~KSP_DS();

	void read_data(std::istream& in);

};

extern KSP_DS ds8;
extern KSP_DS ds10;
extern KSP_DS ds50;
extern KSP_DS ds100;

void init_from_file(const char *);
void init_from_random();

uint64_t rand_gen(unsigned int N, int mn, int mx, KSP_DS::weight_type* dest);
void gen_file(const char* dest);