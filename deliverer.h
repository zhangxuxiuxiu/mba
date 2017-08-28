#pragma once

#include <unordered_map>
#include <future>
#include <thread>
#include <chrono>

#include "recipient.h"
#include "poster.h"
#include "comm/sync_queue.h"

namespace cmf{
	class Deliverer{
		public:	
			Deliverer( std::unordered_multimap< std::type_index, comm::sp< Recipient > >*  id2recipients_ptr) 
				: m_id2recipients_ptr(id2recipients_ptr){}
			virtual ~Deliverer(){}

			virtual void operator() (const comm::sp<Message>& msg) = 0; 

		protected:
			void doDeliver(const comm::sp<Message>& msg);

		protected:
			std::unordered_multimap< std::type_index, comm::sp< Recipient > >*  m_id2recipients_ptr;
	};	
		
	/*
	 * deliver the message to the final recipient directly
	 * */
	class DirectDeliverer final: public Deliverer{
		public:			
			DirectDeliverer(std::unordered_multimap< std::type_index, comm::sp< Recipient > >*  id2recipients_ptr) 
				: Deliverer(id2recipients_ptr){}

			inline virtual void operator() (const comm::sp<Message>& msg) override { doDeliver(msg); }
	};

	
	/*
	 * deliver the message to the final recipient indirectly
	 * */
	template<int SleepMs>
	class IndirectDeliverer : public Deliverer{
		public:			
			IndirectDeliverer(std::unordered_multimap< std::type_index, comm::sp< Recipient > >*  id2recipients_ptr) 
				: Deliverer(id2recipients_ptr), m_queue_messages(), m_is_open(true){}
			virtual ~IndirectDeliverer(){}

			inline virtual void operator() (const comm::sp<Message>& msg) override {
				m_queue_messages.Push(msg);
			}
			// close the box so that remaining messages won't be delivered any more 
			inline void Close(){ m_is_open = false; }

		protected:
			void roll(){
				comm::sp<Message> current_msg;
				while(m_is_open){
					current_msg = m_queue_messages.Take(); // should return anyway
					if( current_msg ){
						doDeliver(current_msg);	
					} else{
						std::this_thread::sleep_for(std::chrono::milliseconds(SleepMs));	
					}
				}
			}

		protected:
			comm::SyncQueue< comm::sp<Message> >	m_queue_messages;

		private:
			bool									m_is_open;
	};

	/*
	 * deliver the message to the recipients asynchronously
	 * the final dispatching time depends on the background thread
	 * */
	template<int SleepMs>
	class AsyncDeliverer final: public IndirectDeliverer<SleepMs>{
		public:			
			// potential problem : type2recipients should be initialized before starting async thread
			// --> asyncDeliverer subclass should bind recipients before invoking AsyncDeliverer constructor 
			AsyncDeliverer( std::unordered_multimap< std::type_index, comm::sp< Recipient > >*  id2recipients_ptr) : 
				IndirectDeliverer<SleepMs>( id2recipients_ptr){
				m_asyncer = std::async( std::launch::async, [this](){
					this->roll();	
				}); 
			}
			~AsyncDeliverer(){ m_asyncer.get(); }

			inline virtual void operator() (const comm::sp<Message>& msg) override {
				IndirectDeliverer<SleepMs>::operator()(msg);
			}

		private:
			std::future<void>	m_asyncer;
	};

	/*
	 * dispatch the message to the recipients synchronously
	 * the recipients are wrapped-recipients or regional-recipients
	 * */
	template<int SleepMs>
	class SystemDeliverer final: public IndirectDeliverer<SleepMs>{
		public:			
			SystemDeliverer( std::unordered_multimap< std::type_index, comm::sp< Recipient > >*  id2recipients_ptr) 
				: IndirectDeliverer<SleepMs>( id2recipients_ptr){}
			~SystemDeliverer(){}

			// return a poster so that outsider could send messages into the box
			inline Poster GetPoster() { return Poster( &this->m_queue_messages); }
			inline void operator()() {
				this->roll();
			}
			inline virtual void operator() (const comm::sp<Message>& msg) override {
				IndirectDeliverer<SleepMs>::operator()(msg);
			}

	};
	
} 
