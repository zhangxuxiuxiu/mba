#include "handler_advance.h"

namespace comm
{
/*	void ComposableHandler::doStart()
	{
		for( auto& i2h : m_index2HandlerMapper)
			i2h.second.Start();
	}

	void ComposableHandler::Stop()
	{
		for( auto& i2h : m_index2HandlerMapper)
			i2h.second.Stop();
	}

	void SyncHandler::Start()
	{
		ComposableHandler::Start();
	}

	void SyncHandler::Stop()
	{
		ComposableHandler::Stop();
	}
*/
	void SyncHandler::Handle( sp<Messag> const& msg ) 
	{
		m_index2HandlerMapper[msg->Type()].Handle( msg);
	}


	void AsyncHandler::Start()
	{
		Register<CloseMessage>([]( CloseMessage const& msg){ throw std::exception("close async Handler");});

		auto runner = [&]()
		{
			try
			{
				while(true)
				{
					auto msg = m_qMsgQueue.Take();
					m_index2HandlerMapper[msg->Type()].Handle( msg);
				}
			}
			catch( std::exception ex) // CloseMessage will throw exception to break the loop
			{ }
		}
		m_spAsyncer = std::make_shared<std::thread>(runner); 
	}

/*	void AsyncHandler::Stop()
	{
		GetSender().Send<CloseMessage>();
		ComposableHandler::Stop();
	}
*/
	void AsyncHandler::Handle( sp<Message> const& msg ) 
	{
		m_qMsgQueue.Push( msg);	
	}
} // end of comm
