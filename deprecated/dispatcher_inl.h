#pragma once

#include "type_indexes_of.h"

namespace comm
{

/*	template< class message, class... args >
	void dispatcher::push( args&&... _args )
	{
		m_qMsgQueue.push( std::make_shared< template_message< message > >( std::forward< args >( _args )... ) ); 
	}
*/	
	
	template< class message, class... args >
	dispatcher& dispatcher::handleWith( args&&... _args)//template_handler< message >&& handler )
	{
		registerHandler( typeid( message ), std::make_shared< template_handler< message > >( std::forward<args>(_args)... ) );
		return *this;
	}
	
	template< class... messages >
	dispatcher& dispatcher::handleWith( sp<handler_base> const& sub_handler  )
	{
		for( auto& ti : comm::type_indexes_of<messages...>() )			
			registerHandler( ti, sub_handler );
		return *this;
	}

} // end of comm
