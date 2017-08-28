#pragma once

#include <future>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <iostream>

#include "sender.h"
#include "handler.h"

namespace mba 
{
	/*
	 * compose handlers through registration 
	 * */
	class ComposableHandler : public Handler
	{
		public:
			ComposableHandler( Sender msgCenter) : m_msgCenter( msgCenter),
				Handler(){} 
			virtual ~ComposableHandler(){
				std::cout << "destructor in composer\n";	
			}

			// leave it for subclass to include those subHandlers
			virtual void Init() override final;
			// start another thread if the handler is asynchrounous.
			virtual void Start() override final;

			virtual void Handle(const comm::sp<Message>& msg ) override = 0;

		protected:
			// compose this compound handler through registering smaller ones
			virtual void compose() = 0;
			// run syncly or asyncly
			virtual void run() = 0;

			
			// register handler for a specific msg type
			template< class Msg, class... Args >
			ComposableHandler& Register( Args&&... args)
			{
				return doRegister( typeid( Msg ), make_handler<Msg>( std::forward<Args>(args)... ) );
			}
			// register composableHandler for a series of msg types
			ComposableHandler& Register( comm::sp<ComposableHandler> const& handler  );

			// do real handling, rather than pass msg to other handlers 
			void doHandle(const comm::sp<Message>& msg);

		private:
			// register a Handler with a specific message type_index
			ComposableHandler& doRegister( std::type_index const& ti, comm::sp<Handler> const& handler);

		protected:
			Sender														m_msgCenter;

		private:
			// map each msg's type_index to its handlers
			std::unordered_multimap< std::type_index, comm::sp< Handler > >	m_index2HandlersMapper;
			// all types which the whole handlers here in this registry can handle
			std::unordered_set<std::type_index>							m_setMessages;
			// all handlers in this composable handler
			std::unordered_set<comm::sp< Handler> >						m_setHandlers;
	};

	class LoopRunner
	{
		public:	
			virtual ~LoopRunner(){} // make it inheritable
			inline Sender GetSender() { return Sender( &m_qMsgQueue); }

			template<class F>
			void operator()( F&& f)//std::function<void(comm::sp<Message>)> const& processor)//  )
			{
				std::function<void(comm::sp<Message> const&)> processor( std::forward<F>(f) );// = std::bind( std::forward<Args>(args)..., std::placeholders::_1 );
				try
				{
					comm::sp<Message>	msg;
					while( (msg = m_qMsgQueue.Take()) )
					{
						std::cout << "start to process msg\n";
						processor( msg);
					}
				}
				catch(...) 
				{
					std::cout << "got an exception, loop in handler finishes\n";	
				}
			}

			void Stop()
			{	m_qMsgQueue.Push( comm::sp<Message>() ); }

			void Push( comm::sp<Message> const& msg)
			{ m_qMsgQueue.Push(msg); }

		private:
			comm::SyncQueue< comm::sp<Message> >		m_qMsgQueue;
	};
	
	class SyncHandler : public ComposableHandler
	{
		public:
			SyncHandler( Sender msgCenter ) : ComposableHandler( msgCenter){ }
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
			SubHandler( Sender msgCenter ) : SyncHandler( msgCenter ){} 	
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
			// so a default sender is provided
			// besides Stop is provided for outside to terminate the system
			SystemHandler() : SyncHandler( Sender() )
			{ Register<Stop>( [this]( Stop const&){ this->m_loopRunner.Stop(); } );}
			
			Sender GetSender() { return m_loopRunner.GetSender(); }

			// make it inheritable
			virtual ~SystemHandler(){}	

		protected:
			virtual void compose() override = 0;
			virtual void run() override final;
	};

	class AsyncHandler : public ComposableHandler//, public LoopRunner
	{
		private:
			LoopRunner			m_loopRunner;
			// stay in destruction order
			std::future<void>	m_asyncer;

		public:
			AsyncHandler( Sender msgCenter) : ComposableHandler(msgCenter){} 

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

