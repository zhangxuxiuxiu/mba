#pragma once

#include <typeindex>
#include <exception>
#include <functional>
#include <unordered_set>
#include <string>

#include "message.h"
#include "util/demangle.h"

namespace cmf 
{

	class Recipient : private Noncopyable
	{
		public:
			virtual void operator()(const sptr<Message>& msg ) = 0;
			virtual ~Recipient() {};
	};
	
	namespace { // WrappedRecipient should be invisible to the outside
		template< class MsgType > 
		class WrappedRecipient final : public Recipient
		{
			public:
				template< class... Args >
				WrappedRecipient( Args&&... args) : Recipient(), 
					m_functor( std::bind(std::forward<Args>(args)..., std::placeholders::_1) ) {}
				~WrappedRecipient(){}
		
				virtual void operator()(const sptr<Message>& msg ) override final
				{
					auto wrapped_msg_ptr = dynamic_cast<  WrappedMessage<MsgType>*  >( msg.get() );		
					if( wrapped_msg_ptr != nullptr ){
						m_functor( static_cast<MsgType const&>(*wrapped_msg_ptr) ); 
					} else {
						throw std::invalid_argument( std::string("wrong Recipient with type=") + demangle<MsgType>()
								+ " is mapped to message type=" + msg->Info() );
					}
				}
		
			private:
				std::function< void(const MsgType& ) > m_functor;
		};
	}

	template<class MsgType, class... Args>
	sptr<Recipient> make_recipient( Args&&... args)
	{
		return std::make_shared< WrappedRecipient<MsgType>>( std::forward<Args>(args)... );	
	}

} // end of comm

