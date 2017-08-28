#pragma once

#include <future>
#include <iostream>

#include "regional_office.h"

namespace mba 
{


	class LoopRunner
	{
		public:	
			virtual ~LoopRunner(){} // make it inheritable
			inline Poster GetSender() { return Poster( &m_qMsgQueue); }

			template<class F>
			void operator()( F&& f)//std::function<void(comm::sp<Message>)> const& processor)//  )
			{
		//		std::function<void(comm::sp<Message> const&)> processor( std::forward<F>(f) );// = std::bind( std::forward<Args>(args)..., std::placeholders::_1 );
				try
				{
					comm::sp<Message>	msg;
					while( (msg = m_qMsgQueue.Take()) )
					{
						std::cout << "start to process msg\n";
		//				processor( msg);
						f(msg);
					}
				}
				catch(...) 
				{
					std::cout << "got an exception, loop in Recipient finishes\n";	
				}
			}

			void Stop()
			{	m_qMsgQueue.Push( comm::sp<Message>() ); }

			void Push( comm::sp<Message> const& msg)
			{ m_qMsgQueue.Push(msg); }

		private:
			comm::SyncQueue< comm::sp<Message> >		m_qMsgQueue;
	};
	
	class SyncHandler : public RegionalOffice
	{
		public:
			SyncHandler( Poster msgCenter ) : RegionalOffice( msgCenter){ }
			virtual ~SyncHandler(){}	
	
			virtual void Handle(const comm::sp<Message>& msg ) override final
			{
				doHandle( msg);
			}

		protected:
			virtual void compose() override = 0;
			virtual void run() override = 0; 
	};

	class SubHandler : public SyncHandler
	{
		public:
			SubHandler( Poster msgCenter ) : SyncHandler( msgCenter ){} 	
			virtual ~SubHandler(){};	

		protected:
			virtual void compose() override = 0;
			virtual void run() override final {}	// do nothing
	};


	struct Stop{}; 
	class SystemHandler	: public SyncHandler 
	{
		private:
			LoopRunner	m_loopRunner;

		public:
			// system handler won't communicate with outsider
			// so a default Poster is provided
			// besides Stop is provided for outside to terminate the system
			SystemHandler() : SyncHandler( Poster() )
			{ Register<Stop>( [this]( Stop const&){ this->m_loopRunner.Stop(); } );}
			
			Poster GetSender() { return m_loopRunner.GetSender(); }

			// make it inheritable
			virtual ~SystemHandler(){}	

		protected:
			virtual void compose() override = 0;
			virtual void run() override final;
	};

	class AsyncHandler : public RegionalOffice//, public LoopRunner
	{
		private:
			LoopRunner			m_loopRunner;
			// stay in destruction order
			std::future<void>	m_asyncer;

		public:
			AsyncHandler( Poster msgCenter) : RegionalOffice(msgCenter){} 

			virtual ~AsyncHandler()
			{
				m_loopRunner.Stop();
				std::cout << "async handler destructed" << std::endl;
			}	
	
			virtual void Handle(const comm::sp<Message>& msg ) override final
			{	
				m_loopRunner.Push( msg);	
			}

		protected:
			// register its subhandlers
			virtual void compose() override = 0;
			virtual void run() override final;

	};
}

