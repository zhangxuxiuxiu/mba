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
		ATMHardware( Poster poster) : LocalOffice( poster){
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
								this->m_poster.emplace<TransactionOver>();	
								std::cout << "after success in deposition\n";	
							});	
		} 
		~ATMHardware(){}
};


class Bank final : public AsyncOffice
{
	public:	
		Bank( Poster poster) : AsyncOffice( poster){
			std::cout << "bank is initializing...\n";
			bind<Credential>([this]( Credential const&) 
				{ 
					std::cout << "before credential got verified\n";
					this->m_poster.emplace<SuccessOnDeposit>(); 
					std::cout << "after credential got verified\n";
				});
			bind<TransactionOver>([this]( TransactionOver const&)
				{
					std::cout << "before transactio-over got verified\n";
					this->m_poster.emplace<CmfClose>(); 
					std::cout << "after transaction-over got verified\n";
				}); 
		}
		~Bank() {
		}
};


class SimpleATM final: public HeadOffice
{
	public:
		SimpleATM() {
			std::cout << "simple atm is initializing...\n";
			bind( std::make_shared<ATMHardware>( GetPoster() ) );	
			bind( std::make_shared<Bank>( GetPoster() ) );	
			std::cout << "simple atm got initialized...\n";
		}
		~SimpleATM(){};
};

int main(int argc, char* argv[])
{
	SimpleATM atm;
//	atm.GetPoster().emplace<StartOnDeposit>();
	atm( make_message<StartOnDeposit>() );
	atm.Open();
	std::cout << "transaction is over\n";

	return 0;
}
