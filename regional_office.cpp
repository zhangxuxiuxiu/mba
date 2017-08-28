#include "regional_office.h"

namespace cmf{
	
	RegionalOffice& RegionalOffice::bind(comm::sp< Recipient> const& recipient) 
	{
		for( auto& ti : recipient->MessageTypes() ) {
			m_set_messages.emplace(ti);
	//		std::cout << "Recipient=" << typeid(recipient).name() << " got registered\n";
			m_map_index2recipients.emplace( ti, recipient); 
		}
		
		return *this;
	}

	RegionalOffice& RegionalOffice::bind(comm::sp< AsyncOffice> const& asyncOffice) 
	{
		bind( static_cast<comm::sp<Recipient>>(asyncOffice) );
		(*asyncOffice)(); // start asyncOffice
	}

	void RegionalOffice::doDeliver(const comm::sp<Message>& msg)
	{
		auto range = m_map_index2recipients.equal_range(msg->Type());
		// deliver the message to the corresponding recipients 
		for(auto it = range.first; it != range.second ; ++it ){
//			std::cout << "processing " << typeid(msg.get()).name() << " in " << __PRETTY_FUNCTION__  << '\n';
			(*it->second)(msg);
		}
	}

}
