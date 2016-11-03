#pragma once

#include <typeinfo>

namespace comm
{

	class Message
	{
		public:
			virtual ~Message(){}
			virtual const std::type_info& Type() const = 0;
	};
	
	template< class Msg > 
	class WrappedMessage : public Message
	{
		public:
			template< class... Args>
			WrappedMessage( Args&&... args) : m_msg( std::forward<Args>(args)... ) {}
			~WrappedMessage(){}	
	
			virtual const std::type_info& Type() const override final
			{
				return typeid( Msg );
			}
	
	//		const msg& get() 
	//		{ 
	//			return m_msg; 
	//		}
			operator Msg() const 
			{
				return m_msg;
			}
	
			//non-copyable
			WrappedMessage( WrappedMessage& msg) = delete;
			WrappedMessage  operator=( const WrappedMessage& msg ) = delete;
	
		private:
			Msg m_msg;
	};

} // end of comm
