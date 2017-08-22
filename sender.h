#pragma once

#include "comm/sync_queue.h"
#include "comm/comm.h"
#include "msg.h"

namespace mba 
{
	class Sender
	{
		public:
			Sender() : m_pMsgQueue( nullptr) {};
			Sender( comm::SyncQueue< comm::sp<Message> >* p_queue) : m_pMsgQueue( p_queue){}

			Sender( Sender const&) = default;
			Sender& operator=( Sender const&) = default;

			template< class Msg, class... Args >
			void Send( Args&&... args )
			{
				Send( make_msg<Msg>( std::forward<Args>(args)... ) ); 
			}

			void Send( comm::sp<Message> const& msg )
			{
				if( nullptr != m_pMsgQueue)
					m_pMsgQueue->Push( msg );
				else
					throw std::logic_error(" messege queue in the Sender is null");  
			}

		private:
			comm::SyncQueue< comm::sp<Message> >* m_pMsgQueue;
	};

} // end of comm
