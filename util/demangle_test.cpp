#include "demangle.h"

#include <iostream>

using namespace cmf;
using namespace std;

int main(){
	cout << demangle(typeid(int)) << '\n';	
	cout << demangle<int>() << '\n';	
	cout << demangle<int const>() << '\n';	
	cout << demangle<int&>() << '\n';	
	cout << demangle<int const&>() << '\n';	
	cout << demangle(3) << '\n';	
	int i = 4;
	cout << demangle(i) << '\n';	
	const int j = 4;
	cout << demangle(j) << '\n';	

	//special cases
	cout << demangle(typeid(std::type_info)) << '\n';	
	cout << demangle<std::type_info>() << '\n';	

	return 0;
}
