#pragma once

#include <thread>
#include <future>
#include <atomic>

#include "regional_office.h"

namespace cmf{
	/*
	 * dispatch real message to the final recipients
	 * */
	class LocalOffice: public RegionalOffice{
		public:	
			LocalOffice() : RegionalOffice(){}
			virtual ~LocalOffice(){}	

			inline virtual void operator()(const sp<Message>& msg ) override final{
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
					bind<CmfStop>([this](CmfStop const&){this->close();});	
				}
			virtual ~ProxyOffice(){};

			inline virtual void operator() (sp<Message> const& msg) override final{
				m_queue_messages.Push(msg);
			}

		protected:
			inline void dispatch(){
				sp<Message> current_msg;
				ms delay_ms(0);
				while(m_is_open.load(std::memory_order_consume)){ // only sync the m_is_open in $close
					current_msg = m_queue_messages.Take(); // should return anyway
					if( current_msg ){
						delay_ms = current_msg->AriseLaterMs();
						if(delay_ms.count() <= 0){
							doDeliver(current_msg);	
							std::cout << "delayMs is=" << delay_ms.count() << '\n';
						} else {
							(*this)(current_msg);	
							std::this_thread::sleep_for(ms(std::min(m_sleep_ms, delay_ms)));	
						} 
					} else { 
						std::this_thread::sleep_for(m_sleep_ms);	
					}
				}
			}

			// for ProxyOffice bind, the should set parameter's poster
			inline virtual RegionalOffice& bindOffice( sp<RegionalOffice> const& subOffice) override{
				RegionalOffice::bindOffice(subOffice);	
				// setting this so that sub-office could post message to current office through m_poster
				subOffice->m_poster = &m_queue_messages;
				return *this; 
			}

		protected:
			SyncQueue< sp<Message> >	m_queue_messages;

		private:
			inline void close(){ m_is_open.store(false, std::memory_order_release);} // just to sync the $dispatch

			std::atomic<bool>						m_is_open;
			const ms								m_sleep_ms;
	};

	/*
	 * dispatch messages to async thread
	 * */
	class AsyncOffice : public ProxyOffice{
		public:	
			explicit AsyncOffice(ms const& sleepMs = ms(100) ) : ProxyOffice(sleepMs){}
			virtual ~AsyncOffice(){ m_asyncer.get(); }	

		private:
			inline virtual void roll() override final{
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
			explicit HeadOffice(ms const& sleepMs = ms(100)) : ProxyOffice(sleepMs){}
			virtual ~HeadOffice(){}	
	
			inline Poster GetPoster() { return Poster( &this->m_queue_messages); }
			inline void Run() {
				dispatch();	
			}
	};

}
