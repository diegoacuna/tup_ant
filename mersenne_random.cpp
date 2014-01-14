#include "mersenne_random.h"

MersenneRandom::MersenneRandom()
{
}

MersenneRandom::MersenneRandom(uint32_t seed) : seed_(seed)
{
	gen_.seed(seed_);
}

void MersenneRandom::initialize_generator(uint32_t seed)
{
	seed_ = seed;
	gen_.seed(seed_);
}

void MersenneRandom::create_uniform_int_dis(int min, int max)
{
	int_uniform_dis_.param(std::uniform_int_distribution<int>::param_type{min, max});
	this->int_min = min;
	this->int_max = max;
}

int MersenneRandom::uniform_int_number()
{
	return int_uniform_dis_(gen_);
}

void MersenneRandom::create_uniform_real_dis(double min, double max)
{
	real_uniform_dis_.param(std::uniform_real_distribution<double>::param_type{min, max});
	this->double_max = min;
	this->double_max = max;
}

double MersenneRandom::uniform_real_number()
{
	return real_uniform_dis_(gen_);
}

MersenneRandom::~MersenneRandom()
{
}

bool MersenneRandom::check_construction_of_int_dis(int min, int max)
{
	if(this->int_min == min && this->int_max == max)
		return false;
	return true;
}

bool MersenneRandom::check_construction_of_double_dis(double min, double max)
{
	if(this->double_min == min && this->double_max == max)
		return false;
	return true;
}
