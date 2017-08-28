#include "poster.h"
#include "recipient.h"

#include <unordered_map>

namespace cmf{

	/*
	 *  a regional office which could process multiple message types
	 *  by delivering each message to its corresponding recipient
	 * */
	class RegionalOffice : public Recipient
	{
		public:
			RegionalOffice( Poster poster = Poster() ) : Recipient(), m_poster(poster){} 
			virtual ~RegionalOffice(){}

		protected:
			// bind Recipient for a specific msg type
			template< class MsgType, class... Args >
			inline RegionalOffice& bind( Args&&... args)
			{
				return bind( make_recipient<MsgType>( std::forward<Args>(args)... ) );
			}
			// bind recipient for a series of msg types
			RegionalOffice& bind( comm::sp<Recipient> const& recipient  );
			class AsyncOffice;
			// bind asyncOffice for a series of msg types
			RegionalOffice& bind( comm::sp<AsyncOffice> const& asyncOffice);
			// deliver the msg to the final recipient
			void doDeliver(const comm::sp<Message>& msg);


		protected:
			Poster																m_poster;
			// map each msg's type_index to its Recipients
			std::unordered_multimap< std::type_index, comm::sp< Recipient > >	m_map_index2recipients;
			// all Recipients in this composable Recipient --> avoid duplicate recipients
		//	std::unordered_set<comm::sp< Recipient> >							m_set_recipients;
	};
}



