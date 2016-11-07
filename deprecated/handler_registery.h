#pragma once

#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include "handler.h"

namespace comm
{
	class ComposableHandler;

	class HandlerRegistery
	{
		public:
			HandlerRegistery() = default;
			virtual ~HandlerRegistery() {}

			// register handler for a specific msg type
			template< class Message, class... Args >
			HandlerRegistery& Register( Args&&... args);
			// register composableHandler for a series of msg types
			//template< class... Messages >
			HandlerRegistery& Register( sp<ComposableHandler> const& handler  );

			// obtain corresponding handler with the typeIndex
			sp<Handler> GetHandler( std::type_index const&);
			// return all types that the all handlers can handle in this registery 
			inline std::unordered_set<std::type_index> const HandlableTypes() const { return m_setHandlableTypes; }
	
		private:
			// register a Handler with a specific message type_index
			void registerHandler( std::type_index const& ti, sp<Handler> const& handler);
			// all types which the whole handlers here in this registry can handle
			std::unordered_set<std::type_index> m_setHandlableTypes;

		protected:
			std::unordered_map< std::type_index, sp< Handler > >	m_index2HandlerMapper;
	};

} // end of comm

#include "handler_registery_inl.h"
