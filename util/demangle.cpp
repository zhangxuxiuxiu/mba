#include "demangle.h"

#include <cxxabi.h>

namespace cmf{

	template<>
	std::string demangle(const std::type_info& type){
		  int status;
		  size_t len = 0;
		  // malloc() memory for the demangled type name
		  char* demangled = abi::__cxa_demangle(type.name(), nullptr, &len, &status);
		  if (status != 0) {
		    return type.name();
		  }
		  return demangled;
	}

}
