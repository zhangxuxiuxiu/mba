#include "regional_office.h"

#include <stdexcept>

namespace cmf{
	
	RegionalOffice& RegionalOffice::bindOffice(sp<RegionalOffice> const& office) 
	{
		for( auto& ti : office->m_set_messages){
			m_set_messages.emplace(ti);
			m_map_index2recipients.emplace( ti, office); 
		}
		// actually only AsyncOffice really do sth here
		office->roll(); 

		return *this;
	}

	void RegionalOffice::doDeliver(const sp<Message>& msg)
	{
		auto range = m_map_index2recipients.equal_range(msg->Type());
		if( range.first == range.second){ 
			if( not m_poster ){ // only HeadOffice's m_poster is false
				throw std::logic_error( msg->Info() + " does not bind to any recipients" );
			} else{ // deliver the msg to its parent to process 
				m_poster(msg); 
			}
		}
		// deliver the message to the corresponding recipients 
		for(auto it = range.first; it != range.second ; ++it ){
			(*it->second)(msg);
		}
	}

}
