#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

namespace comm
{
	template< class Tp >
	class SyncQueue  
	{
		public:
			void Push( const Tp& ele);
			// if nothing to take, wait 
			Tp Take();

		private:
			std::mutex					m_mutex;
			std::condition_variable		m_cond;
			std::queue< Tp >			m_queue;  
	};

}   // end of comm

#include "sync_queue_inl.h"
