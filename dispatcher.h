#pragma once

#include "handler.h"
#include "sync_queue.h"
#include "sender.h"
#include "handler_registery.h"

namespace comm
{
	class dispatcher : public handler_base, HandlerRegistery
	{
		public:
			dispatcher( uint32_t delayInMs = 1000 ) : m_iBreakInMs( delayInMs ){}
			~dispatcher(){}
	
			virtual void start() override;
			inline virtual void stop() override{ m_bStopFlag	= true; }
			inline sender get_sender() { return sender( &m_qMsgQueue); }
			virtual void handle( const sp< message_base >& msg ) override final;

		private:
			uint32_t	m_iBreakInMs;
			std::atomic_bool	m_bStopFlag;
			sync_queue< sp<message_base> >	m_qMsgQueue;
	};

} // end of comm

#include "dispatcher_inl.h"
