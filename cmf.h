#pragma once

#include <thread>
#include <future>
#include <atomic>
#include <vector>
#include <algorithm>

#include "regional_office.h"

namespace cmf{
	/*
	 * dispatch real message to the final recipients
	 * */
	class LocalOffice: public RegionalOffice{
		public:	
			LocalOffice() : RegionalOffice(){}
			virtual ~LocalOffice(){}	

			inline virtual void operator()(const sptr<Message>& msg ) override final{
				doDeliver(msg);
			}	
	};

	struct CmfStop{}; // message to stop proxy offices
	/*
	 * a proxy to dispatch messages to other offices or recipients
	 * */
	class ProxyOffice : public RegionalOffice{
		public:	
			explicit ProxyOffice(ms const& sleepMs ) : RegionalOffice(), 
				m_is_open(true), m_sleep_ms(sleepMs){
					// bind the closer to CmfClose message
					bind([this](CmfStop const&){this->close();});	
				}
			virtual ~ProxyOffice(){};

			inline virtual void operator() (sptr<Message> const& msg) override final{
				m_queue_messages.Push(msg);
			}

		protected:
			inline void dispatch(utl::pcq<sptr<Message>>& msg_queue){
				sptr<Message> current_msg;
				while(m_is_open.load(std::memory_order_consume)){ // only sync the m_is_open in $close
					if( msg_queue.Pop(current_msg) ){
						ms delay_ms = current_msg->AriseLaterMs();
						if(delay_ms.count() <= 0){
							doDeliver(current_msg);	
						} else { // repush the message until its arise-time
							msg_queue.Push(current_msg);	
							std::this_thread::sleep_for(ms(std::min(m_sleep_ms, delay_ms)));	
						} 
					} 
			/*
					auto should_deliver = [](sptr<Message> const& msg){
								return msg->AriseLaterMs().count() <= 0; }; 
					if( m_queue_messages.Pop(current_msg, should_deliver ) ){
							doDeliver(current_msg);	
					} else { 
						std::this_thread::sleep_for(m_sleep_ms);	
					}
			*/
				}
			}

			// for ProxyOffice bind, the should set parameter's poster
			inline virtual RegionalOffice& bindOffice( sptr<RegionalOffice> const& subOffice) override{
				RegionalOffice::bindOffice(subOffice);	
				// setting this so that sub-office could post message to current office through m_poster
				subOffice->m_poster = &m_queue_messages;
				return *this; 
			}

		protected:
	//		SyncQueue< sptr<Message> >	m_queue_messages;
			utl::pcq< sptr<Message> >	m_queue_messages;
			std::atomic<bool>			m_is_open;
			const ms					m_sleep_ms;

		private:
			inline void close(){ m_is_open.store(false, std::memory_order_release);} // just to sync the $dispatch

	};

	/*
	 * dispatch messages to async thread
	 * */
	class AsyncNOffice : public ProxyOffice{
		public:	
			explicit AsyncNOffice(size_t parallels=1, ms const& sleepMs = ms(100)) 
				: ProxyOffice(sleepMs), m_parallels(parallels), m_queues(parallels-1){
				if(parallels < 1){
					throw std::logic_error("parallels in AsyncNOffice can't be less than one\n");
				} 
			}
			~AsyncNOffice(){
				std::for_each(m_asyncers.begin(),m_asyncers.end(),[](std::future<void>& fut){ fut.get(); });	
//				std::cout << m_asyncers.size() << " asyncers have been joined\n";
			}
	
		private:
			inline virtual void roll() override final{
				if(m_parallels ==1){
					m_asyncers.push_back( std::async( std::launch::async, [this](){
						this->dispatch(this->m_queue_messages); }) );	
				} else {
					// push msg to each queue in turn
					m_asyncers.push_back( std::async( std::launch::async, 
					[this](){ 
						size_t turn = 0, queue_size = this->m_queues.size();	
						sptr<Message> current_msg;
						while( this->m_is_open.load(std::memory_order_consume) ){
							if(this->m_queue_messages.Pop(current_msg) ){
								this->m_queues[turn++].Push(current_msg);	
								turn %= queue_size;
							} else {
								std::this_thread::sleep_for(ms(this->m_sleep_ms));	
							}
						}
					} ) );

				// run each async thread to do real job
					for(auto& msg_queue : m_queues){
						m_asyncers.push_back( std::async( std::launch::async, 
							[this,&msg_queue](){ this->dispatch(msg_queue); } ) );
					}						
				} 
			}

		private:
			size_t									m_parallels;
			std::vector<utl::pcq<sptr<Message>>>	m_queues;
			std::vector<std::future<void>>			m_asyncers;
	};
/*
	class AsyncOffice : public ProxyOffice{
		public:	
			explicit AsyncOffice(ms const& sleepMs = ms(100) ) : ProxyOffice(sleepMs){}
			virtual ~AsyncOffice(){ m_asyncer.get(); }	

		private:
			inline virtual void roll() override final{
				m_asyncer = std::async( std::launch::async, [this](){
					this->dispatch(this->m_queue_messages);	
				}); 
			}
		
		private:
			std::future<void>	m_asyncer;
	};
*/
	/*
	 * globally negotiate message dispatching
	 * */
	class HeadOffice : public ProxyOffice{
		public:	
			// HeadOffice does not need a poster to send message to other recipients
			explicit HeadOffice(ms const& sleepMs = ms(100)) : ProxyOffice(sleepMs){}
			virtual ~HeadOffice(){}	
	
			inline Poster GetPoster() { return Poster( &this->m_queue_messages); }
			inline void Run() {
				dispatch(this->m_queue_messages);	
			}
	};

}
