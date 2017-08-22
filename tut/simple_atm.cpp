#include "../handler_advance.h"
#include <iostream>

using namespace mba;

struct StartOnDeposit{};
struct Credential{};
struct SuccessOnDeposit{};
//struct TransactionOver{};

class ATMHardware final: public SubHandler
{
	public:
		ATMHardware( Sender msgCenter) : SubHandler( msgCenter){} 
		~ATMHardware(){}
	
	protected:
		virtual void compose() override final
		{
			std::cout << "atm hardware is initializing...\n";
			Register<StartOnDeposit>([this]( StartOnDeposit const&) 
							{ 
								std::cout << "start to depoist\n";
								this->m_msgCenter.Send<Credential>(); 
								std::cout << "finish to depoist\n";
							});
			Register<SuccessOnDeposit>([this]( SuccessOnDeposit const&)
							{
								std::cout << "success in deposition\n";	
								this->m_msgCenter.Send<Stop>();	
								std::cout << "after success in deposition\n";	
							});	
							
		}
};


class Bank final : public AsyncHandler
{
	public:	
		Bank( Sender msgCenter) : AsyncHandler( msgCenter){}
		~Bank() {// std::cout << "bank destructed\n"; 
		}
	
	protected:
		virtual void compose() override final
		{
			std::cout << "bank is initializing...\n";
			Register<Credential>([this]( Credential const&) 
				{ 
					std::cout << "before credential got verified\n";
					this->m_msgCenter.Send<SuccessOnDeposit>(); 
					std::cout << "after credential got verified\n";
				});
		}
};


class SimpleATM final: public SystemHandler
{
	public:
		SimpleATM() = default;
		~SimpleATM(){};

	protected:
		virtual void compose() override final
		{
			std::cout << "simple atm is initializing...\n";
			Register( std::make_shared<ATMHardware>( GetSender() ) );	
			Register( std::make_shared<Bank>( GetSender() ) );	
			std::cout << "simple atm got initialized...\n";
		}
};

int main(int argc, char* argv[])
{
	SimpleATM atm;
	std::cout << "begins to initialize...\n";
	atm.Init();
	std::cout << "begins to start...\n";
	atm.GetSender().Send<StartOnDeposit>();
	atm.Start();
	std::cout << "transaction is over\n";

	return 0;
}
