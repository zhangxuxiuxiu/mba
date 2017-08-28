#pragma once

#include <unordered_map>

#include "recipient.h"

namespace cmf{

	template<class DeliverStrategy>
	class MessageBox{
		public:
			MessageBox(std::unordered_multimap< std::type_index, comm::sp< Recipient > >* id2recipients_ptr) :
				m_queue_messages(), m_message_deliverer(&m_queue_messages, id2recipients_ptr), m_is_open(true){}

			// close the box so that remaining messages won't be delivered any more 
			inline void Close(){ m_is_open = false; }

			// doing message delivering job according to the specific deliver-strategy
			void operator(const comm::sp<Message>& msg)(){
				while(m_is_open){
					m_message_deliver(msg);
				}
			}

		private:
			comm::SyncQueue< comm::sp<Message> >		m_queue_messages;
			DeliverStrategy								m_message_deliverer;
			bool										m_is_open;
	};

}
