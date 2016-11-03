#pragma once

#include "comm.h"
#include "msg.h"
#include <typeindex>
#include <exception>
#include <functional>
#include <string>

namespace comm
{

	class Handler
	{
		public:
			virtual void Handle(const sp<Message>& msg ) = 0;
			virtual void Start(){};  // empty default implementation means subclass can ignore this
			virtual void Stop(){};
			virtual ~Handler() {};
	};
	
	template< class Msg  > 
	class WrappedHandler : public Handler
	{
		public:
			template< class... Args >
			WrappedHandler( Args&&... args) : m_f( std::forward<Args>(args)... ){}
			~WrappedHandler(){}
			// non-copyable
			WrappedHandler( WrappedHandler& handler ) = delete;
			WrappedHandler& operator=( const WrappedHandler& handler ) = delete;
	
			virtual void Handle(const sp<Message>& msg ) override final
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

} // end of comm

