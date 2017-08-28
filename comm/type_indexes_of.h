#pragma once

#include "variadic.h"
#include <typeindex>
#include <vector>

namespace comm
{
	template<class T>
	class TypeIndexesHook
	{
		public:
			void operator()( std::vector< std::type_index>&	vecTypeIndexes )
			{
				vecTypeIndexes.push_back( typeid(T));
			}
	};

	template<class... Types>
	std::vector< std::type_index> type_indexes_of()
	{
		std::vector< std::type_index> result;
		detail::variadic_np< TypeIndexesHook, Types...>()( result);	
		return result;
	}

} // end of comm

