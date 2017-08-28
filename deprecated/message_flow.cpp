#include "handler_advance.h"

namespace mba 
{

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
