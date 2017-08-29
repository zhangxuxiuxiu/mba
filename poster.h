#pragma once

#include "message.h"
#include "util/sync_queue.h"
#include "util/util.h"


namespace cmf 
{
	class Poster
	{
		public:
			//support implicit conversion
			Poster( SyncQueue< sptr<Message> >* p_queue = nullptr) : m_queue_messages_ptr( p_queue){}

			Poster( Poster const&) = default;
			Poster( Poster &&) = default;
			Poster& operator=( Poster const&) = default; 
			Poster& operator=( Poster &&) = default; 

			inline operator bool() const { return nullptr != m_queue_messages_ptr;}

			template< class MsgType, class... Args >
			inline void emplace( Args&&... args ){
				operator()( make_message<MsgType>( std::forward<Args>(args)... ) ); 
			}

			void operator()( sptr<Message> const& msg ){
				if( m_queue_messages_ptr != nullptr) {
					m_queue_messages_ptr->Push( msg );
				} else {
					throw std::logic_error(" messege queue in the Poster is null");  
				}
			}

			void operator()( sptr<Message> && msg ){
				if( m_queue_messages_ptr != nullptr) {
					m_queue_messages_ptr->Push( std::move(msg) );
				} else {
					throw std::logic_error(" messege queue in the Poster is null");  
				}
			}

		private:
			SyncQueue< sptr<Message> >* m_queue_messages_ptr;
	};

} // end of cmf 
