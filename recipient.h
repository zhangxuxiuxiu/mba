#pragma once

#include <typeindex>
#include <exception>
#include <functional>
#include <unordered_set>
#include <string>

#include "message.h"

namespace cmf 
{

	class Recipient : private comm::Noncopyable
	{
		public:
			virtual void operator()(const comm::sp<Message>& msg ) = 0;
			virtual ~Recipient() {};

	};
	
	namespace { // WrappedRecipient should be invisible to the outside
		template< class MsgType > 
		class WrappedRecipient final : public Recipient
		{
			public:
				template< class... Args >
				WrappedRecipient( Args&&... args) : Recipient(), m_functor( std::bind(std::forward<Args>(args)..., std::placeholders::_1) ) {
	//				m_set_messages.insert(typeid(MsgType)); 
				}
				~WrappedRecipient(){}
		
				virtual void operator()(const comm::sp<Message>& msg ) override final
				{
					auto wrapped_msg = dynamic_cast<  WrappedMessage<MsgType>*  >( msg.get() );		
					if( wrapped_msg != nullptr ){
						m_functor( *wrapped_msg ); 
					} else {
						throw std::invalid_argument( std::string("wrong Recipient with type=") + typeid( *this ).name() + " is mapped to message type=" + msg->Info() );
					}
				}
		
			private:
				std::function< void(const MsgType& ) > m_functor;
		};
	}

	template<class MsgType, class... Args>
	comm::sp<Recipient> make_recipient( Args&&... args)
	{
		return std::make_shared< WrappedRecipient<MsgType>>( std::forward<Args>(args)... );	
	}

} // end of comm

