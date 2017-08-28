#pragma once

#include <queue>
#include <mutex>

namespace comm
{
	template< class Tp >
	class SyncQueue  
	{
		public:
			void Push( const Tp& ele){
				std::lock_guard<std::mutex> lg(m_mutex);
				m_queue.push( ele );  
			}

			// return front() or default Tp
			Tp Take(){
				std::lock_guard<std::mutex> lg(m_mutex);
				if( m_queue.empty() ){
					return Tp();	
				} else {
					// copy front to res other than that refering to it by auto&  
					// which may crrupt the memory
					auto res=m_queue.front(); 
					m_queue.pop();
					return res;
				}
			}

		private:
			std::mutex					m_mutex;
			std::queue< Tp >			m_queue;  
	};

}   // end of comm

