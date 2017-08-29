#include "message.h"
#include "comm/sync_queue.h"

#include <cassert>

using namespace cmf;

int main(){
	SyncQueue<sp<Message>> q;
	q.Push(make_message<int>(3));
	q.Push(make_message<long>(4));

	auto msg3 = make_message<int>(5);
	std::cout << msg3->AriseTime().time_since_epoch().count() << '\n';
	auto msg4 = msg3->AriseAfter(ms(1000));
	std::cout << msg4->AriseTime().time_since_epoch().count() << '\n';
	q.Push(msg3);
	assert(msg3 == msg4);	

	auto msg = q.Take();
	while(msg){
		std::cout << msg->AriseTime().time_since_epoch().count() << '\n';	
		msg = q.Take();
	}

	return 0;
}
