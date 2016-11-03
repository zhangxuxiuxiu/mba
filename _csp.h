#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace csp
{

struct message_base  // 队列项的基础类
{
	virtual ~message_base(){}
};

template<typename Msg>
struct wrapped_message: message_base
{
	Msg contents;

	explicit wrapped_message(Msg const& contents_): contents(contents_) {}

	~wrapped_message(){}
};

class queue  // 我们的队列
{
	std::mutex m;
	std::condition_variable c;
	std::queue<std::shared_ptr<message_base> > q;  // 实际存储指向message_base类指针的队列

	public:
	template<typename T>
	void push(T const& msg)
	{
		std::lock_guard<std::mutex> lg(m);
		q.push(std::make_shared<wrapped_message<T> >(msg));  // 包装已传递的信息，存储指针
		c.notify_all();
	}

	std::shared_ptr<message_base> wait_and_pop()
	{
		std::unique_lock<std::mutex> lk(m);
		c.wait(lk,[&]{return !q.empty();});  // 当队列为空时阻塞
		auto& res=q.front();
		q.pop();
		return res;
	}
};

template<typename PreviousDispatcher,typename Msg,typename Func>
class TemplateDispatcher
{
	queue* q;
	PreviousDispatcher* prev;
	Func f;
	bool chained; 
	TemplateDispatcher(TemplateDispatcher const&)  = delete;
	TemplateDispatcher& operator=(TemplateDispatcher const&) = delete;

	template<typename Dispatcher,typename OtherMsg,typename OtherFunc>
	friend class TemplateDispatcher;  // 所有特化的TemplateDispatcher类型实例都是友元类

	void wait_and_dispatch()
	{
		for(;;)
		{
			auto msg=q->wait_and_pop();
			if( dispatch(msg) )  // 1 如果消息处理过后，会跳出循环
				break;
		}
	}

	bool dispatch( std::shared_ptr<message_base> const& msg )
	{
		if( wrapped_message<Msg>* wrapper = dynamic_cast< wrapped_message<Msg>* >( msg.get() ) )  // 2 检查消息类型，并且调用函数
		{
			f( wrapper->contents );
			return true;
		}
		else
		{
			return prev == nullptr ? false : prev->dispatch(msg) ;  // 3 链接到之前的调度器上
		}
	}
		
	public:
	TemplateDispatcher( TemplateDispatcher&& other ): q( other.q ), prev( other.prev ), 
		f( std::move(other.f) ), chained( other.chained )
	{
		other.chained = true;
	}

	TemplateDispatcher( queue* q_, PreviousDispatcher* prev_, Func&& f_): q(q_), prev(prev_), 
		f(std::forward<Func>(f_)), chained(false)
	{
		if( prev_ != nullptr )
			prev_->chained=true;
	}

	template<typename OtherMsg,typename OtherFunc>
	TemplateDispatcher< TemplateDispatcher, OtherMsg, OtherFunc> handle( OtherFunc&& of )  // 4 可以链接其他处理器
	{
		return TemplateDispatcher<TemplateDispatcher,OtherMsg,OtherFunc>( q, this, std::forward<OtherFunc>(of) );
	}

	~TemplateDispatcher() noexcept(false)  // 5 这个析构函数也是noexcept(false)的
	{
		if(!chained)
		{
			wait_and_dispatch();
		}
	}
};

// specializing the header dispatcher
class close_queue {};

class close_function
{
	public:
		void operator() ( const close_queue& msg)
		{
			throw close_queue();
		}
};

class NullDispatcher
{
	public:
		bool chained;
		bool dispatch( std::shared_ptr< message_base > const& msg){ return false; }

};
// alias the default dispatcher
using dispatcher = TemplateDispatcher< NullDispatcher, close_queue, close_function >;


class sender
{
	queue*q;  // sender是一个队列指针的包装类

	public:
	sender():  q( nullptr ) {} // sender无队列(默认构造函数)

	explicit sender(queue* q_): q( q_ ) {}  // 从指向队列的指针进行构造

	template<typename Message>
	void send(Message const& msg)
	{
		if(q)
		{
			q->push(msg);  // 将发送信息推送给队列
		}
	}
};


class receiver
{
	queue q;  // 接受者拥有对应队列

	public:
	operator sender()  // 允许将类中队列隐式转化为一个sender队列
	{
		return sender(&q);
	}

	dispatcher wait()  // 等待对队列进行调度
	{
		return dispatcher(&q, nullptr, close_function() );
	}
};


} // end of csp
