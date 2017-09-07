#pragma once

#include "poster.h"
#include "recipient.h"

#include <unordered_map>

namespace cmf{

	/*
	 *  a regional office which could process multiple message types
	 *  by delivering each message to its corresponding recipient
	 * */
	class ProxyOffice; // forward declaration to be friendly to m_poster
	class RegionalOffice : public Recipient
	{
		public:
			RegionalOffice() : Recipient(), m_poster(){} 
			virtual ~RegionalOffice(){}

		protected:
			// bind Recipient for a specific msg type
			template< class Func, class MsgType = typename std::decay<typename utl::function_traits<Func>::template args<0>::type>::type >
			inline auto bind( Func&& func) 
			-> typename std::enable_if<not IsRecipientDefined<MsgType>::value,RegionalOffice&>::type// && not std::is_bind_expression<Func>::value
			{
				return doBind( typeid(MsgType), make_recipient(std::forward<Func>(func)) );
			}
			// support varidic parameters 
			template<class Func, class Arg1, class... Args, 
				class MsgType=typename utl::function_traits<Func>::template args<utl::placeholder_index<1, Arg1,Args...>::value>::type>
			inline auto bind( Func&& func, Arg1&& arg1, Args&&... args) 
			-> typename std::enable_if<not IsRecipientDefined<MsgType>::value, RegionalOffice&>::type
			{
				return doBind( typeid(MsgType), make_recipient(std::bind(std::forward<Func>(func),std::forward<Arg1>(arg1), std::forward<Args>(args)...) ) );
			}
			
			// easy-use interface for binding system-defined message
			template< class MsgType, class RecipientType = typename IsRecipientDefined<MsgType>::Type >
			inline auto bind() 
			-> typename std::enable_if<IsRecipientDefined<MsgType>::value,RegionalOffice&>::type
			{
				return doBind( typeid(MsgType), make_recipient(RecipientType() ) );
			}
			// bind office for a series of msg types
			virtual RegionalOffice& bindOffice( sptr<RegionalOffice> const& office);
			// deliver the msg to the final recipient
			void doDeliver(const sptr<Message>& msg);

		private:
			// start dispatching
			virtual void roll() {};
			
			inline RegionalOffice& doBind( std::type_info const& info, sptr<Recipient> const& recipient_sptr){
				m_set_messages.emplace(info);
				m_map_index2recipients.emplace( info, recipient_sptr); 
				return *this;
			}

		protected:
			friend class ProxyOffice;
			// to be used by ProxyOffice
			Poster														m_poster;

		private:
			// all types which the whole Recipients here in this registry can handle
			std::unordered_set<std::type_index>							m_set_messages;
			// map each msg's type_index to its Recipients
			std::unordered_multimap< std::type_index, sptr<Recipient> >	m_map_index2recipients;
			// all Recipients in this composable Recipient --> avoid duplicate recipients
		//	std::unordered_set<sptr< Recipient> >							m_set_recipients;
	};
}



