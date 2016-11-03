#include "comm.h"
#include <iostream>

int main()
{
	using namespace comm;
	auto simple = make_unique<int>();
	auto unbound_array = make_unique<int[]>(3);
	auto unbound_array2 = make_unique<int[][2]>(3);
//  disabled bounded array 
//	auto bound_array = make_unique<int[2]>(3);

	return 0;
}
