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
	int int_min;
	int int_max;
	double double_min;
	double double_max;
public:
	MersenneRandom();
	MersenneRandom(uint32_t seed);
	void initialize_generator(uint32_t seed);
	void create_uniform_int_dis(int min, int max);
	int uniform_int_number();
	void create_uniform_real_dis(double min, double max);
	double uniform_real_number();
	bool check_construction_of_int_dis(int min, int max);
	bool check_construction_of_double_dis(double min, double max);
	~MersenneRandom();

};

#endif // MERSENNERANDOM_H
