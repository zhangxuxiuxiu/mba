#include "handler_advance.h"

namespace mba 
{
	//######## ComposableHandler ###########//
	void ComposableHandler::Init()
	{
		compose();	
		for( auto& h : m_setHandlers) { h->Init(); }
	}

	void ComposableHandler::Start()
	{
//		compose();
		run();
	}


	ComposableHandler& ComposableHandler::Register( comm::sp<ComposableHandler> const& handler  )
	{
//		handler->Init();
		std::cout << "size of msgs=" << handler->m_setMessages.size() << '\n';
		for( auto& ti : handler->m_setMessages ) 
		{
			doRegister( ti, handler );
		}

		return *this;
	}

	ComposableHandler& ComposableHandler::doRegister( std::type_index const& ti, comm::sp< Handler > const& handler) 
	{
		m_setMessages.emplace(ti);
		// ensure a unique handler won't be inserted more than once
		m_setHandlers.emplace(handler);
		std::cout << "handler=" << typeid(handler).name() << " got registered\n";
		m_index2HandlersMapper.emplace( ti, handler ); 
		
		return *this;
	}

	void ComposableHandler::doHandle(const comm::sp<Message>& msg)
	{
		auto range = m_index2HandlersMapper.equal_range(msg->Type());
		// process this msg by all registered handlers
		for(auto it = range.first; it != range.second ; ++it )
		{
			std::cout << "processing " << typeid(msg.get()).name() << " in " << __PRETTY_FUNCTION__  << '\n';
			it->second->Handle( msg);
		}
	}

	// ####### SystemHandler ###### //
	void SystemHandler::run()
	{
		m_loopRunner( [this](comm::sp<Message> const& msg)
				{ 
					std::cout << "processing " << typeid(msg.get()).name() << " in " << __PRETTY_FUNCTION__  << '\n';
					this->doHandle(msg);
				} );
	}

	//####### AsyncHandler ########//
	void AsyncHandler::run()
	{
		m_asyncer = std::async( std::launch::async, std::ref(m_loopRunner),  
					[this] (comm::sp<Message> const& msg)
					{ 
						this->doHandle(msg); 
					} ); 
	}

} // end of comm
