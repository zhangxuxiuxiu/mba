#pragma once

#include "handler_registery.h"
//#include "dispatcher.h"
#include "sender.h"
#include "instances_of.h"
#include <thread>

namespace comm
{
	class ComposableHandler : public HandlerRegistery, Handler
	{
		public:
			ComposableHandler( Sender msgCenter) : m_msgCenter( msgCenter){} 
			virtual ~ComposableHandler(){}
			
			virtual void Handle(const sp<Message>& _msg ) override;

			// leave it for subclass to include those subHandlers
			virtual void Init() = 0;

		protected:
			Sender								m_msgCenter;
	};

	class SyncHandler : ComposableHandler
	{
		public:
			SyncHandler( Sender msgCenter) : ComposableHandler( msgCenter){ compose(); }
			~SyncHandler(){}	
	
			virtual void Handle(const sp<Message>& msg ) override final;

			virtual void Init() override =0;
	};


	class AsyncHandler : ComposableHandler
	{
		public:
			AsyncHandler( Sender msgCenter = Sender()) : ComposableHandler(msgCenter){}
			~AsyncHandler(){}	
	
			// just as the interface for handler, so it cat act as a subHandler
			virtual void Handle(const sp<Message>& msg ) override final;
			// act as a door for outsider to send message to this handler
			inline Sender GetSender() { return Sender( &m_qMsgQueue); }

			virtual void Init() override =0;
			// start processing messages asynchronously
			void Start();

		private:
			SyncQueue< sp<Message> >		m_qMsgQueue;
			sp< std::thread>				m_spAsyncer;

			struct CloseMessage{};
	};

/*
	class DispatchableHandler : public handler_base	
	{
		public:
			DispatchableHandler( sender msgCenter) : m_processor(), m_selfSender( m_processor.get_sender()), m_msgCenter( msgCenter){}
			DispatchableHandler() : m_processor(), m_selfSender( m_processor.get_sender()){}
			~DispatchableHandler(){}

			std::unordered_set<std::type_index> const handlableTypes() const { return m_setHandlableTypes; }
			virtual void handle(const sp< message_base >& _msg ) override final { m_selfSender.send( _msg );}
			virtual void start() override{ dispatch(); m_processor.start(); }
			virtual void stop() override { m_processor.stop(); }
					
		protected:
			virtual void dispatch() = 0;

			template< class... Msgs>
			void markHandlable() 
			{ 
				for( auto& ti : type_indexes_of< Msgs...>() )
					m_setHandlableTypes.insert( ti);
			}

			dispatcher	m_processor;
			sender		m_selfSender;		
			sender		m_msgCenter;
			std::unordered_set<std::type_index> m_setHandlableTypes;
	};

	struct close_msg{};

	template<class... DispatchableHandlers>
	class MasterHandler : public DispatchableHandler	
	{
		public:
			MasterHandler() : DispatchableHandler(), m_vecSubHandlers( instances_of< DispatchableHandler, DispatchableHandlers...>( m_selfSender) ) {}
			~MasterHandler(){}

			virtual void start()  override final
			{
				// start all the sub handlers as async threads
				for( auto& sub : m_vecSubHandlers)	
				{	
					m_vecSubThreads.emplace_back( &DispatchableHandler::start, sub.get() );	
				}
				DispatchableHandler::start();
			}
	
			virtual void stop()  override final
			{
				for( auto& sub : m_vecSubHandlers)	
					sub->stop();
				for( auto& sub : m_vecSubThreads)	
				{
					if( sub.joinable() )	
							sub.join();
				}
				DispatchableHandler::stop();
			}
	
			virtual void dispatch() override final
			{
				for( auto& sub : m_vecSubHandlers)	
					m_processor.handleWith( sub->handlableTypes(), sub);
				m_processor.handleWith< close_msg >( [this]( close_msg const&){ this->stop(); } );
			}

		private:
			std::vector< sp<DispatchableHandler>>	m_vecSubHandlers;
			std::vector< std::thread >	m_vecSubThreads;
	};
*/

}
