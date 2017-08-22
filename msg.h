#pragma once

#include <typeinfo>

#include "comm/comm.h" 
#include "comm/noncopyable.h"

namespace mba 
{
	class Message : public comm::Noncopyable
	{
		public:
			virtual ~Message(){}
			virtual const std::type_info& Type() const = 0;
	};
	
	template< class Msg > 
	class WrappedMessage final: public Message
	{
		public:
			template< class... Args>
			WrappedMessage( Args&&... args) : m_msg( std::forward<Args>(args)... ) {}
			~WrappedMessage(){}	
	
			virtual const std::type_info& Type() const override final
			{
				return typeid(Msg);
			}
	
			operator Msg const& () noexcept 
			{
				return m_msg;
			}
	
		private:
			Msg m_msg;
	};

	template<class Msg, class... Args>
	std::shared_ptr<Message> make_msg( Args&&... args)
	{
		return std::make_shared<WrappedMessage<Msg>>( std::forward<Args>(args)... );	
	}

} // end of comm
