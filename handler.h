#pragma once

#include "comm/comm.h"
#include "msg.h"
#include <typeindex>
#include <exception>
#include <functional>
#include <string>

namespace mba 
{

	class Handler : public  comm::Noncopyable
	{
		public:
			// do preparation for handling message
			virtual void Init() = 0;
			virtual void Start() = 0;
			virtual void Handle(const comm::sp<Message>& msg ) = 0;
			virtual ~Handler() {};
	};
	
	template< class Msg > 
	class WrappedHandler final : public Handler
	{
		public:
			template< class... Args >
			WrappedHandler( Args&&... args) : m_f( std::forward<Args>(args)... ){}
			~WrappedHandler(){}
	
			virtual void Init() override {};
			virtual void Start() override {}
			virtual void Handle(const comm::sp<Message>& msg ) override final
			{
				auto _msg = dynamic_cast<  WrappedMessage<Msg>*  >( msg.get() );		
				if( _msg != nullptr )
				{
					m_f( *_msg ); // msg->get()
				}
				else
				{
					throw std::invalid_argument( std::string("wrong handler with type=") + typeid( *this ).name() + " is mapped to message type=" + typeid(Msg).name() );
				}
			}
	
		private:
			std::function< void(const Msg& ) > m_f;
	};

	template<class Msg, class... Args>
	comm::sp<Handler> make_handler( Args&&... args)
	{
		return std::make_shared< WrappedHandler<Msg>>( std::forward<Args>(args)... );	
	}

} // end of comm

