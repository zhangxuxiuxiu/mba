#include "message.h"
#include "util/sync_queue.h"

#include <cassert>
#include <iostream>

using namespace cmf;
using namespace utl;

int main(){
	// 1) test make_message<T>(Args...);
	SyncQueue<sptr<Message>> q;
	q.Push(make_message<int>(3));
	q.Push(make_message<long>(4));

	auto msg3 = make_message<int>(5);
	std::cout << msg3->AriseTime().time_since_epoch().count() << '\n';
	auto msg4 = msg3->AriseAfter(ms(1000));
	std::cout << msg4->AriseTime().time_since_epoch().count() << '\n';
	q.Push(msg3);
	assert(msg3 == msg4);	

	sptr<Message> msg;
	while(q.Pop(msg)){
		std::cout << msg->AriseTime().time_since_epoch().count() << '\n';	
	}

	// 2) test sptr<Message> as make_message parameter
	auto msg_six = make_message<uint32_t>(6);
	auto msg_sptr = make_message(msg_six);				
	assert(msg_six==msg_sptr);

	// 3) test msg object 
	auto msg_eight=make_message(8);
	auto msg_eight2=make_message<decltype(8)>(8);

	// 4) test orderedMessage
	auto ordered_msg =make_message(ordering(std::string("str_alike"), 89));

	return 0;
}
