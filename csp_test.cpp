#include "handler_advance.h"
#include <iostream>

using namespace comm;
using namespace std;

// msg types
struct msg_v1{};

struct msg_v2
{
	msg_v2( int v){}
};

struct msg_v3
{
	msg_v3( int v){}
};

struct msg_v4{};
struct msg_v5{};
struct msg_v6{};

// handlers
class v2Op
{
	public:
		void operator() ( const msg_v2& _msg)
		{
			std::cout << "handled msg with type=" << typeid( _msg ).name() << "\n";		
		}
};

void v3_func( const msg_v3& _msg)
{
	std::cout << "handled msg with type=" << typeid( _msg ).name() << "\n";		
}

class v4Op
{
	public:
		void handle( const msg_v4& _msg)
		{
			std::cout << "handled msg with type=" << typeid( _msg ).name() << "\n";		
		}
};

class CompntTwo: public DispatchableHandler 
{
	public:
		CompntTwo( sender msg_center) : DispatchableHandler( msg_center){}; 
		~CompntTwo(){}

		virtual void dispatch() override final
		{
				
		}
};


// sub services
class CompntOne : public DispatchableHandler
{
	public:
		CompntOne( sender msg_center) : DispatchableHandler( msg_center){}; 
		~CompntOne(){}

		virtual void dispatch() override final
		{
			//v4Op v4op;
			m_processor.handleWith< msg_v1 >(
			[&]( const msg_v1& _msg )
			{
				std::cout << "handled msg with type=" << typeid( _msg ).name() << "\n";		
				std::cout << "sending msg_v5 to other party\n";		
				m_msgCenter.send<msg_v5>();
			}
			)
		  .handleWith< msg_v2 >( v2Op() )
		  .handleWith< msg_v3 >( v3_func )
		  //.handle< msg_v4 >( bind( mem_fn(&v4Op::handle), ref(v4op), placeholders::_1 ) );
		  .handleWith< msg_v4 >( bind( &v4Op::handle, v4Op(), placeholders::_1 ) ); // &v4op works here too, so what does mem_fn do?
			markHandlable< msg_v1, msg_v2, msg_v3, msg_v4>();	
		}
};


int main( int argc, char* argv[])
{
//	MasterHandler< CompntOne, CompntTwo> system;
//	system.get_sender().send<msg_v1>();
//	system.start();
	/*dispatcher system(1000);
	dispatcher otherParty(1000);
	
	// provide different handler for different message type

	auto sub_handler = std::make_shared<SubHandler>();
	otherParty.handleWith< msg_v5, msg_v6>( sub_handler);

	thread system_worker( &dispatcher::start, &system );
	thread other_worker( &dispatcher::start, &otherParty);

	auto sender = system.get_sender();
	sender.send< msg_v1 >(   );
	sender.send< msg_v2 >( 1 );
	sender.send< msg_v3 >( 1 );
	sender.send< msg_v4 >(   );

	system_worker.join();
	other_worker.join();
	*/		
	AsyncHandler	system;

	auto sender = system.GetSender()
	SyncHandler		subOne(sender);
	subOne.RegisterWith<msg_v1>( [&]( msg_v1 const& msg) 
			{
				std::cout << "handled msg with type=" << typeid( _msg ).name() << "\n";		
				std::cout << "sending msg_v5 to other party\n";		
				sender.Send<msg_v5>();
			}
		);

	AsyncHandler	subTwo( system.GetSender() );
	subTwo.RegisterWith<msg_v5>([&]( msg_v5 const& msg)
					{
						std::cout << "msg_v5 got handled\n";	
					}	
				);

	system.RegisterWith<>

	return 0;
}
