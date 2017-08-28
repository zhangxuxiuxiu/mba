#pragma once

#include <thread>
#include <future>
#include <atomic>

#include "regional_office.h"
//#include "deliverer.h"

namespace cmf{
	/*
	 * dispatch real message to the final recipients
	 * */
	class LocalOffice: public RegionalOffice{
		public:	
			LocalOffice(Poster poster) : RegionalOffice(poster){}
			virtual ~LocalOffice(){}	

			inline virtual void operator()(const comm::sp<Message>& msg ) override final{
				doDeliver(msg);
			}	
	};

	struct CmfClose{}; // message to stop proxy offices
	/*
	 * a proxy to dispatch messages to other offices or recipients
	 * */
	class ProxyOffice : public RegionalOffice{
		public:	
			ProxyOffice( Poster poster, int sleepMs ) : RegionalOffice(poster), 
				m_is_open(true), m_sleep_ms(sleepMs){
					// bind the closer to CmfClose message
					bind<CmfClose>([this](CmfClose const&){this->close();});	
				}
			virtual ~ProxyOffice(){};
			inline virtual void operator() (comm::sp<Message> const& msg) override final{
				m_queue_messages.Push(msg);
			}
			// start dispatching
			virtual void Open() = 0;

		protected:
			inline void dispatch(){
				comm::sp<Message> current_msg;
				while(m_is_open.load(std::memory_order_consume)){ // only sync the m_is_open in $close
					current_msg = m_queue_messages.Take(); // should return anyway
					if( current_msg ){
						doDeliver(current_msg);	
					} else{
						std::this_thread::sleep_for(std::chrono::milliseconds(m_sleep_ms));	
					}
				}
			}

		protected:
			comm::SyncQueue< comm::sp<Message> >	m_queue_messages;

		private:
			inline void close(){ m_is_open.store(false, std::memory_order_release);} // just to sync the $dispatch

			std::atomic<bool>						m_is_open;
			const int								m_sleep_ms;
	};
	/*
	 * dispatch messages to async thread
	 * */
	class AsyncOffice : public ProxyOffice{
		public:	
			AsyncOffice(Poster poster, int sleepMs = 100) : ProxyOffice(poster, sleepMs){}
			virtual ~AsyncOffice(){ m_asyncer.get(); }	

			virtual void Open() override final{
				m_asyncer = std::async( std::launch::async, [this](){
					this->dispatch();	
				}); 
			}
		
		private:
			std::future<void>	m_asyncer;
	};

	/*
	 * globally negotiate message dispatching
	 * */
	class HeadOffice : public ProxyOffice{
		public:	
			// HeadOffice does not need a poster to send message to other recipients
			HeadOffice(int sleepMs = 100) : ProxyOffice(Poster(), sleepMs){}
			virtual ~HeadOffice(){}	
	
			inline Poster GetPoster() { return Poster( &this->m_queue_messages); }
			inline virtual void Open() override final{
				dispatch();	
			}
	};

}
