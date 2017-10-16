
#pragma once

#include <typeinfo>
#include <string>
#include <queue>
#include <chrono>
#include <functional>

#include "ptr.h" 
#include "demangle.h"
#include "variadic.h"
#include "noncopyable.h"

namespace cmf 
{
	using ns		= std::chrono::nanoseconds;
	using ps		= std::chrono::microseconds;
	using ms		= std::chrono::milliseconds;
	using sec		= std::chrono::seconds;
	using time_point= std::chrono::steady_clock::time_point;

	inline time_point Now() { return std::chrono::steady_clock::now(); }

	class Message : private utl::Noncopyable, public std::enable_shared_from_this<Message>
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
			inline utl::sptr<Message> AriseAfter(Duration const& duration){
				m_arise_time = Now() + duration;	
				return shared_from_this();
			} 
			// deliver this message at $timeSinceEpochMs 
			inline utl::sptr<Message> AriseAt(time_point const& ariseTime){
				m_arise_time = ariseTime;	
				return shared_from_this();
			} 
			// how long it will be delivered
			inline ms AriseLaterMs() const{
				return std::chrono::duration_cast<ms>(m_arise_time - Now());
			}

			friend bool operator< ( utl::sptr<Message> const& lhs, utl::sptr<Message> const& rhs);

		private:
			time_point	m_arise_time;
	};

	// make it comparable to use in priority_queue
	inline bool operator< ( utl::sptr<Message> const& lhs, utl::sptr<Message> const& rhs){
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
		
				inline virtual std::type_info const& Type() const override {
					return typeid(MsgType);
				}
				inline virtual std::string const& Info() const override {
					return s_type_info;
				}
		
				// explicitly support MsgType conversion
				operator MsgType const& () noexcept {
					return m_raw_msg;
				}
		
			private:
				MsgType m_raw_msg;

				static std::string const s_type_info;
		};

		template<class MsgType>
		std::string const WrappedMessage<MsgType>::s_type_info = utl::demangle(typeid(MsgType));
	}

	// support for wrapped message
	template<class MsgType, class... Args>
	inline utl::sptr<Message> make_message( Args&&... args){
		return std::make_shared<WrappedMessage<MsgType>>( std::forward<Args>(args)... );	
	}

	// support for sptr<Message> 
	template<class MsgType,class DecayMsg=typename std::decay<MsgType>::type>
	inline auto make_message(MsgType&& msg )
		->typename std::enable_if<std::is_same<DecayMsg,utl::sptr<Message>>::value, MsgType >::type{
		return std::forward<MsgType>(msg);
	}

	// support for msg object, pointer is forbidden
	template<class MsgType,class DecayMsg=typename std::decay<MsgType>::type>
	inline auto make_message(MsgType&& msg )
		->typename std::enable_if<not std::is_same<DecayMsg,utl::sptr<Message>>::value
							   && not std::is_pointer<DecayMsg>::value, utl::sptr<Message> >::type{
		return std::make_shared<WrappedMessage<typename std::decay<MsgType>::type>>( std::forward<MsgType>(msg) );	
	}


	//if a recipient is defined for a specifi message, 
	//it must be bind through bind<MsgType>() without parameters;
	template<class MessageType>	
	struct IsRecipientDefined : public std::false_type {};

	class FunctionMessage final{
		public:	
			FunctionMessage(std::function<void()> functor) : m_functor(std::move(functor)){}
			void operator()()const{ m_functor(); }

		private:
			std::function<void()>	m_functor;	
	};	

	struct FunctionMessageRecipient{
		inline void operator()(FunctionMessage const& msg){
			msg();	
		}	
	};

	template<>
	struct IsRecipientDefined<FunctionMessage> : public std::true_type{
		using Type = FunctionMessageRecipient;	
	};

	// support for function ordering
	struct OrderedHook{
		OrderedHook(std::queue<utl::sptr<Message>>&	msgQueue) : msg_queue(msgQueue){} 	

		// support for non-ptr
		template<class T, class DecayT=typename std::decay<T>::type>
		auto operator()(T&& val)
		-> typename std::enable_if<not std::is_pointer<T>::value>::type 
		{
			msg_queue.push(make_message(std::forward<T>(val)));	
		}

		std::queue<utl::sptr<Message>>&	msg_queue;	
	};


	/* support ordering messages so that messages are processed in order
	 * currently, to guarantee strict order between messages, the messages
	 * in a ordered message must be able to be processed in the current thread 
	 */
	class OrderedMessage final{
		public:	
			explicit OrderedMessage(std::queue<utl::sptr<Message>> msgQueue)		
				: m_message_queue(std::move(msgQueue)){}
			std::queue<utl::sptr<Message>> const& MessageQueue() const{
				return m_message_queue;	
			}

			template<class MsgType>
			OrderedMessage& Then(MsgType&& msg){
				OrderedHook(m_message_queue)(std::forward<MsgType>(msg));	
				return *this;
			}
		private:
			std::queue<utl::sptr<Message>>	m_message_queue;	
	};

	template<class... MsgTypes>
	inline OrderedMessage ordering(MsgTypes&&... msgs){
		std::queue<utl::sptr<Message>>	msg_queue;	
		utl::vparams(OrderedHook(msg_queue), std::forward<MsgTypes>(msgs)...);	
		return OrderedMessage(std::move(msg_queue));
	}

} // end of cmf 
