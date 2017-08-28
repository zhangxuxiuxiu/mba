#pragma once

#include "message.h"
#include "comm/sync_queue.h"
#include "comm/comm.h"


namespace cmf 
{
	class Poster
	{
		public:
			Poster( comm::SyncQueue< comm::sp<Message> >* p_queue = nullptr) : m_pMsgQueue( p_queue){}

			Poster( Poster const&) = default;
			Poster& operator=( Poster const&) = default;

			template< class MsgType, class... Args >
			inline void emplace( Args&&... args ){
				operator()( make_message<MsgType>( std::forward<Args>(args)... ) ); 
			}

			void operator()( comm::sp<Message> const& msg ){
				if( m_pMsgQueue) {
					m_pMsgQueue->Push( msg );
				} else {
					throw std::logic_error(" messege queue in the Poster is null");  
				}
			}

		private:
			comm::SyncQueue< comm::sp<Message> >* m_pMsgQueue;
	};

} // end of comm
