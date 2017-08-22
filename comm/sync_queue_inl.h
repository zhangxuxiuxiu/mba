#pragma once

namespace comm
{

	template< class Tp>
	void SyncQueue<Tp>::Push( const Tp& ele)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_queue.push( ele );  
		m_cond.notify_one();
	}
	
	template< class Tp>
	Tp SyncQueue<Tp>::Take()
	{
		std::unique_lock<std::mutex> ul(m_mutex);
		m_cond.wait( ul, [&](){ return !m_queue.empty(); } );
		auto& res=m_queue.front();
		m_queue.pop();
		return res;
	}
}// end of comm
