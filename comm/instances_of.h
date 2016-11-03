#pragma once

#include "../../variadic.h"

namespace comm
{
	template<class T>
	class InstancesHook
	{
		public:	
			template<class BaseType, class... Args>
			void operator()(std::vector< sp<BaseType>>& result, Args&&... args )
			{
				result.push_back( std::make_shared<T>(std::forward<Args>(args)...) );
			}
	};

	template< class BaseType, class... Types, class... Args >
	std::vector< sp<BaseType>> instances_of( Args&&... args)
	{
		std::vector< sp<BaseType>>	result;
		detail::variadic_np<InstancesHook, Types...>()( result, std::forward<Args>(args)... );
		return result;
	}

}  // end of comm
