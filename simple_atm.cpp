#include "cmf.h"

#include <iostream>

using namespace cmf;

struct StartOnDeposit{};
struct Credential{};
struct SuccessOnDeposit{};
struct TransactionOver{};
namespace unbound{
	struct UnboundMessage{};
}

class ATMHardware final: public LocalOffice
{
	public:
		ATMHardware() : LocalOffice(){
			std::cout << "atm hardware is initializing...\n";
		//	bind<StartOnDeposit>([this]( StartOnDeposit const&) 
			bind([this]( StartOnDeposit const&) 
							{ 
								std::cout << "start to depoist\n";
								this->m_poster.emplace<Credential>(); 
								std::cout << "finish to depoist\n";
							});
		//	bind<SuccessOnDeposit>([this]( SuccessOnDeposit const&)
			bind([this]( SuccessOnDeposit const&)
							{
								std::cout << "before success in deposition\n";	
								this->m_poster( make_message<TransactionOver>() );
								std::cout << "after success in deposition\n";	
							});	
		} 
		~ATMHardware(){}
};


class Bank final : public AsyncNOffice
{
	public:	
		Bank() : AsyncNOffice(3){
			std::cout << "bank is initializing...\n";
			bind<Credential>( std::bind(&Bank::authenticate, this, std::placeholders::_1) ); 
			bind([this]( TransactionOver const&)
	//		bind<TransactionOver>([this]( TransactionOver const&)
				{
					std::cout << "before transactio-over got verified\n";
					this->m_poster( make_message<CmfStop>()->AriseAfter(sec(3)) );// ns, ps, ms are also suppported 
					std::cout << "after transaction-over got verified \n";
				}); 
		}
		~Bank() {}

	private:
		void authenticate(Credential const&){
			std::cout << "before credential got verified\n";
			this->m_poster.emplace<SuccessOnDeposit>(); 
			std::cout << "after credential got verified\n";
		}
};



class SimpleATM final: public HeadOffice
{
	public:
		SimpleATM() {
			std::cout << "simple atm is initializing...\n";
			bindOffice( std::make_shared<ATMHardware>() );  // bindOffice could also be used in subclasses of LocalOffice and AsyncOffice
			bindOffice( std::make_shared<Bank>() );	
			std::cout << "simple atm got initialized...\n";
		}
		~SimpleATM(){};
};

int main(int argc, char* argv[])
{
	SimpleATM atm;
//	atm.GetPoster().emplace<StartOnDeposit>();
	atm( make_message<StartOnDeposit>() );
	atm( make_message<StartOnDeposit>()->AriseAfter(ms(2000)) );
//	atm( make_message<unbound::UnboundMessage>() ); //---> caught exception that not recipients bound to type int
	atm.Run();
	std::cout << "transaction is over\n";

	return 0;
}
