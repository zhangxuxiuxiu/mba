#pragma once

#include <queue>
#include <mutex>

namespace cmf 
{
	template< class Tp >
	class SyncQueue  
	{
		public:
			void Push( Tp const& ele){
				std::lock_guard<std::mutex> lg(m_mutex);
				m_queue.push( ele );  
			}
			void Push( Tp && ele){
				std::lock_guard<std::mutex> lg(m_mutex);
				m_queue.push( std::move(ele) );  
			}

			// return front() or default Tp
			bool Pop(Tp& item){
				std::lock_guard<std::mutex> lg(m_mutex);
				if( m_queue.empty() ){
					return false;	
				} else {
					// copy front to res other than that refering to it by auto&  
					// which may crrupt the memory
					item = std::move(m_queue.top()); 
					m_queue.pop();
					return true;
				}
			}

		private:
			std::mutex					m_mutex;
			std::priority_queue< Tp >	m_queue;  
	};

}   // end of comm

