#ifndef MERSENNERANDOM_H
#define MERSENNERANDOM_H

#include <random>
#include <cstdint>

class MersenneRandom
{
private:
	std::mt19937 gen_;
	std::uniform_int_distribution<int> int_uniform_dis_;
	std::uniform_real_distribution<double> real_uniform_dis_;
	uint32_t seed_;
public:
	MersenneRandom();
	MersenneRandom(uint32_t seed);
	void initialize_generator(uint32_t seed);
	void create_uniform_int_dis(int min, int max);
	int uniform_int_number();
	void create_uniform_real_dis(double min, double max);
	double uniform_real_number();
	~MersenneRandom();

};

#endif // MERSENNERANDOM_H
