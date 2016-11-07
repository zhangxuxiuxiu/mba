#pragma once

//#include "type_indexes_of.h"
namespace comm
{
	template< class Message, class... Args >
	HandlerRegistery& HandlerRegistery::Register( Args&&... args)
	{
		registerHandler( typeid( Message ), std::make_shared< WrappedHandler< Message > >( std::forward<Args>(args)... ) );
		return *this;
	}
	
//	template< class... Messages >
	HandlerRegistery& HandlerRegistery::Register( sp<ComposableHandler> const& handler  )
	{
		for( auto& ti : handler->HandlableTypes() ) 
			registerHandler( ti, handler );
		return *this;
	}


} // end of comm
