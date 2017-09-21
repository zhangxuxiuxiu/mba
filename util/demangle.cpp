#include "demangle.h"

#include <cxxabi.h>
#include <cstdlib>

namespace cmf{

	template<>
	std::string demangle(const std::type_info& type){
		  int status;
		  // malloc() memory for the demangled type name
		  char* demangled = abi::__cxa_demangle(type.name(), nullptr, nullptr, &status);
		  std::string dname = (status==0) ? demangled : type.name();
		  std::free(demangled);
		  return std::move(dname);
	}

}
