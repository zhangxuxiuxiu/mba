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
			template< class MsgType, class Func, class... Args >
			inline auto bind( Func&& func, Args&&... args) 
			-> typename std::enable_if<not IsRecipientDefined<MsgType>::value,RegionalOffice&>::type
			{
				m_set_messages.emplace(typeid(MsgType));
				m_map_index2recipients.emplace( typeid(MsgType), 
						make_recipient<MsgType>( std::forward<Func>(func), std::forward<Args>(args)... )); 
				return *this;
			}
			// easy-use interface for binding system-defined message
			template< class MsgType, class RecipientType = typename IsRecipientDefined<MsgType>::Type >
			inline auto bind() 
			-> typename std::enable_if<IsRecipientDefined<MsgType>::value,RegionalOffice&>::type
			{
				m_set_messages.emplace(typeid(MsgType));
				m_map_index2recipients.emplace( typeid(MsgType), 
						make_recipient<MsgType>( RecipientType() )); 
				return *this;
			}
			// bind office for a series of msg types
			virtual RegionalOffice& bindOffice( sptr<RegionalOffice> const& office);
			// deliver the msg to the final recipient
			void doDeliver(const sptr<Message>& msg);

		private:
			// start dispatching
			virtual void roll() {};

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



