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
			Tp Take(){
				std::lock_guard<std::mutex> lg(m_mutex);
				if( m_queue.empty() ){
					return Tp();	
				} else {
					// copy front to res other than that refering to it by auto&  
					// which may crrupt the memory
					auto res=std::move(m_queue.top()); 
					m_queue.pop();
					return std::move(res);
				}
			}

		private:
			std::mutex					m_mutex;
			std::priority_queue< Tp >	m_queue;  
	};

}   // end of comm

