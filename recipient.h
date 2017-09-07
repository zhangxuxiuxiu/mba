#pragma once

#include <typeindex>
#include <exception>
#include <functional>
#include <unordered_set>
#include <string>

#include "message.h"
#include "util/demangle.h"
#include "util/traits.h"

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
				template< class F>
				WrappedRecipient( F&& f): Recipient(), 
					m_functor( std::forward<F>(f) ) {}
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

	// for lambda, std::function, operator() defined Class or std::bind expression, MsgType can be deduced 
	template<class Func, class MsgType = typename std::decay<typename utl::function_traits<Func>::template args<0>::type>::type >
	inline sptr<Recipient > make_recipient( Func&& func)
	{
		return std::make_shared< WrappedRecipient<MsgType>>( std::forward<Func>(func) );	
	}

	// Func in make_recipient could be constructed through std::bind using _1;
	using std::placeholders::_1;

} // end of comm

