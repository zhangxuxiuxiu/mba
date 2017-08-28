#pragma once

#include <typeinfo>

#include "comm/comm.h" 
#include "comm/noncopyable.h"

namespace cmf 
{
	class Message : private comm::Noncopyable
	{
		public:
			virtual ~Message(){}
			virtual const std::type_info& Type() const = 0;
	};
	
	namespace{  // WrappedMessage should be invisible to outside
		template< class MsgType > 
		class WrappedMessage final: public Message
		{
			public:
				template< class... Args>
				WrappedMessage( Args&&... args) : m_raw_msg( std::forward<Args>(args)... ) {}
				~WrappedMessage(){}	
		
				virtual const std::type_info& Type() const override final {
					return typeid(MsgType);
				}
		
				// support MsgType conversion
				operator MsgType const& () noexcept {
					return m_raw_msg;
				}
		
			private:
				MsgType m_raw_msg;
		};
	}

	template<class MsgType, class... Args>
	std::shared_ptr<Message> make_message( Args&&... args)
	{
		return std::make_shared<WrappedMessage<MsgType>>( std::forward<Args>(args)... );	
	}

} // end of comm
