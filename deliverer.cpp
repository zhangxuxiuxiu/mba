#include "deliverer.h"

namespace cmf{

	void Deliverer::doDeliver(const comm::sp<Message>& msg)
	{
		auto range = m_id2recipients_ptr->equal_range(msg->Type());
		// deliver the message to the corresponding recipients 
		for(auto it = range.first; it != range.second ; ++it ){
//			std::cout << "processing " << typeid(msg.get()).name() << " in " << __PRETTY_FUNCTION__  << '\n';
			(*it->second)(msg);
		}
	}

}
