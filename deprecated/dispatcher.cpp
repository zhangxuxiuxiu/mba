#include "dispatcher.h"
#include "../../stl_util.h"
#include <thread>

namespace comm
{

	void dispatcher::start()
	{
		m_bStopFlag	= false;
		while( !m_bStopFlag )
		{
			auto msg = m_qMsgQueue.take();
			if( msg )
			{
				handle( msg );
			}
			else
			{
				std::this_thread::sleep_for( std::chrono::milliseconds( m_iBreakInMs ) );	
			}
		}
	}
	
/*	void dispatcher::handleWith( std::unordered_set<std::type_index> const& types, sp<handler_base> const& sub_handler  )
	{
		for( auto& ti : types)	
			registerHandler( ti, sub_handler);
	}
*/

	void dispatcher::handle( const sp< message_base >& msg ) 
	{
		if( !find( m_mapIndex2HandlerMapper, msg->type() ) )
		{
			throw std::invalid_argument( std::string("there is no corresponding handler attached to message type=") + msg->type().name() );
		}
		else
		{
			m_mapIndex2HandlerMapper[ msg->type() ]->handle( msg );
		}
	}
	
	void dispatcher::registerHandler( std::type_index const& ti, sp< handler_base > const& handler) 
	{
		if( !find( m_mapIndex2HandlerMapper, ti ) )
		{
			m_mapIndex2HandlerMapper.emplace( ti, handler ); 
		}
		else
		{
			throw std::invalid_argument(std::string("the handler is already registered") );
		}
	}

} // end of comm
