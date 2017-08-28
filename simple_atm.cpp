#include "cmf.h"

#include <iostream>

using namespace cmf;

struct StartOnDeposit{};
struct Credential{};
struct SuccessOnDeposit{};
struct TransactionOver{};

class ATMHardware final: public LocalOffice
{
	public:
		ATMHardware() : LocalOffice(){
			std::cout << "atm hardware is initializing...\n";
			bind<StartOnDeposit>([this]( StartOnDeposit const&) 
							{ 
								std::cout << "start to depoist\n";
								this->m_poster.emplace<Credential>(); 
								std::cout << "finish to depoist\n";
							});
			bind<SuccessOnDeposit>([this]( SuccessOnDeposit const&)
							{
								std::cout << "success in deposition\n";	
								this->m_poster( make_message<TransactionOver>() );
								std::cout << "after success in deposition\n";	
							});	
		} 
		~ATMHardware(){}
};


class Bank final : public AsyncOffice
{
	public:	
		Bank() : AsyncOffice(){
			std::cout << "bank is initializing...\n";
			bind<Credential>( &Bank::authenticate, this); 
			bind<TransactionOver>([this]( TransactionOver const&)
				{
					std::cout << "before transactio-over got verified\n";
					this->m_poster( make_message<CmfStop>() ); 
					std::cout << "after transaction-over got verified\n";
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
			bindOffice( std::make_shared<ATMHardware>(  ) );//GetPoster()	
			bindOffice( std::make_shared<Bank>(  ) );//GetPoster()	
			std::cout << "simple atm got initialized...\n";
		}
		~SimpleATM(){};
};

int main(int argc, char* argv[])
{
	SimpleATM atm;
//	atm.GetPoster().emplace<StartOnDeposit>();
	atm( make_message<StartOnDeposit>() );
//	atm( make_message<int>() ); //---> caught exception that not recipients bound to type int
	atm.Run();
	std::cout << "transaction is over\n";

	return 0;
}
