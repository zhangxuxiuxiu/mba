#pragma once

#include "sync_queue.h"
#include "msg.h"

namespace comm
{
	class Sender
	{
		public:
			Sender() : m_pMsgQueue( nullptr) {};
			Sender( SyncQueue< sp<Message> >* p_queue) : m_pMsgQueue( p_queue){}

			Sender( Sender const&) = default;
			Sender& operator=( Sender const&) = default;

			template< class Msg, class... Args >
			void Send( Args&&... args )
			{
				Send( std::make_shared< WrappedMessage<Msg> >( std::forward<Args>(args)... ) ); 
			}

			void Send( sp<Message> const& msg )
			{
				if( nullptr != m_pMsgQueue)
					m_pMsgQueue->push( msg );
				else
					throw std::logic_error(" messege queue in the Sender is null");  
			}

		private:
			SyncQueue< sp<Message> >* m_pMsgQueue;
	};

} // end of comm
