#pragma once

#include <typeinfo>
#include <string>

#include "comm/comm.h" 
#include "comm/noncopyable.h"

namespace cmf 
{
	class Message : private comm::Noncopyable
	{
		public:
			virtual ~Message(){}
			virtual std::type_info const& Type() const = 0;
			virtual std::string const& Info() const = 0;
	};
	
	namespace{  // WrappedMessage should be invisible to outside
		template< class MsgType > 
		class WrappedMessage final: public Message
		{
			public:
				template< class... Args>
				WrappedMessage( Args&&... args) : m_raw_msg( std::forward<Args>(args)... ) {}
				~WrappedMessage(){}	
		
				inline virtual std::type_info const& Type() const override final {
					return typeid(MsgType);
				}
				inline virtual std::string const& Info() const override final {
					return s_info;
				}
		
				// explicitly support implicit MsgType conversion
				operator MsgType const& () noexcept {
					return m_raw_msg;
				}
		
			private:
				MsgType m_raw_msg;

				static std::string const s_info;
		};

		template<class MsgType>
		std::string const WrappedMessage<MsgType>::s_info = typeid(MsgType).name();
	}

	template<class MsgType, class... Args>
	comm::sp<Message> make_message( Args&&... args)
	{
		return std::make_shared<WrappedMessage<MsgType>>( std::forward<Args>(args)... );	
	}

} // end of cmf 
