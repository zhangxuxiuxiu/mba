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

			// return top() if not empty 
	/*		bool Pop(Tp& item){
				std::lock_guard<std::mutex> lg(m_mutex);
				if( m_queue.empty() ){
					return false;	
				} else {
					item = std::move(m_queue.top()); 
					m_queue.pop();
					return true;
				}
			}
*/
			// return if top() meets the requirement of &pred
			bool Pop(Tp& item, std::function<bool(Tp const&)> const& pred =[](Tp const&){ return true;}){
				std::lock_guard<std::mutex> lg(m_mutex);
				if( m_queue.empty() || !pred(m_queue.top()) ){
					return false;	
				} else {
					item = std::move(m_queue.top()); 
					m_queue.pop();
					return true;
				}
			
			}

		private:
			std::mutex					m_mutex;
			std::priority_queue< Tp >	m_queue;  
	};

}   // end of cmf 

