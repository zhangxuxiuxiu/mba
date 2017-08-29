#pragma once

#include <typeinfo>
#include <string>
#include <chrono>

#include <iostream>

#include "util/util.h" 
#include "util/demangle.h"
#include "util/noncopyable.h"

namespace cmf 
{
	using ns		= std::chrono::nanoseconds;
	using ps		= std::chrono::microseconds;
	using ms		= std::chrono::milliseconds;
	using sec		= std::chrono::seconds;
	using time_point= std::chrono::steady_clock::time_point;

	inline time_point Now() { return std::chrono::steady_clock::now(); }

	class Message : private Noncopyable, public std::enable_shared_from_this<Message>
	{
		public:
			Message( time_point const& ariseTime = Now() ) 
				: m_arise_time( ariseTime){}
			virtual ~Message(){}

			virtual std::type_info const& Type() const = 0;
			virtual std::string const& Info() const = 0;

			inline time_point const& AriseTime () const { return m_arise_time; }
			// deliver this message $delayMs later
			template<class Duration>
			inline sptr<Message> AriseAfter(Duration const& duration){
				m_arise_time = Now() + duration;	
				return shared_from_this();
			} 
			// deliver this message at $timeSinceEpochMs 
			inline sptr<Message> AriseAt(time_point const& ariseTime){
				m_arise_time = ariseTime;	
				return shared_from_this();
			} 
			// how long it will be delivered
			inline ms AriseLaterMs() const{
				return std::chrono::duration_cast<ms>(m_arise_time - Now());
			}

			friend bool operator< ( sptr<Message> const& lhs, sptr<Message> const& rhs);

		private:
			time_point	m_arise_time;
	};

	// make it comparable to use in priority_queue
	inline bool operator< ( sptr<Message> const& lhs, sptr<Message> const& rhs){
		return lhs->m_arise_time > rhs->m_arise_time;	// earlier message should stay at top of priority_queue 
	}

	namespace{  // WrappedMessage should be invisible to outside
		template< class MsgType > 
		class WrappedMessage final: public Message
		{
			public:
				template< class... Args>
				WrappedMessage( Args&&... args) : m_raw_msg( std::forward<Args>(args)... ) {}
				~WrappedMessage(){}	
		
				inline virtual std::type_info const& Type() const override final {
					return typeid(MsgType);
				}
				inline virtual std::string const& Info() const override final {
					return s_type_info;
				}
		
				// explicitly support implicit MsgType conversion
				operator MsgType const& () noexcept {
					return m_raw_msg;
				}
		
			private:
				MsgType m_raw_msg;

				static std::string const s_type_info;
		};

		template<class MsgType>
		std::string const WrappedMessage<MsgType>::s_type_info = demangle(typeid(MsgType));
	}

	template<class MsgType, class... Args>
	sptr<Message> make_message( Args&&... args){
		return std::make_shared<WrappedMessage<MsgType>>( std::forward<Args>(args)... );	
	}

} // end of cmf 
