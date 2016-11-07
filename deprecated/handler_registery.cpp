#include "handler_registery.h"
#include "../../stl_util.h"

namespace comm
{
	void HandlerRegistery::registerHandler( std::type_index const& ti, sp< Handler > const& handler) 
	{
		if( !find( m_index2HandlerMapper, ti ) )
		{
			m_index2HandlerMapper.emplace( ti, handler ); 
			m_setHandlableTypes.emplace(ti);
		}
		else
		{
			throw std::invalid_argument(std::string("the handler is already registered") );
		}
	}

	sp<Handler> HandlerRegistery::GetHandler( std::type_index const& ti)
	{
		auto it = find( m_index2HandlerMapper, ti ); 
		if(it) 
		{
			return it.get();
		}
		else  
		{
			throw std::invalid_argument(std::string("the handler is not registered") );
		}
	}

} // end of comm
