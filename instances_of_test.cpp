#include "instances_of.h"
#include <iostream>

struct base{};
struct derived1 : public base{};
struct derived2 : public base{};

int main(int argc, char* argv[])
{
	auto instances = comm::instances_of< base, derived1, derived2>();
	std::cout << "there are " << instances.size() << " subclasses in vector\n";
	return 0;
}
