#include <thread>
#include <chrono>
#include <iostream>
#include <memory>

class SubModule
{
	public:
		void Start()
		{
			m_thread = std::make_shared<std::thread>(
						[]()	
						{
							auto k = 3;
							while( k-- >0)	
							{
								std::this_thread::sleep_for( std::chrono::milliseconds(1000) );	
								std::cout << "tick in sub thread\n";
							}
						}
					);	
		}

		~SubModule()
		{
			if( m_thread && m_thread->joinable())	
			{
				m_thread->join();	
			}
		}

	private:
		std::shared_ptr<std::thread>	m_thread;
};



int main()
{
	auto runner = []()	
	{
		SubModule().Start();
		auto k = 3;
		while( k-- >0)	
		{
			std::this_thread::sleep_for( std::chrono::milliseconds(1000) );	
			std::cout << "tick in main thread\n";
		}
	};

	runner();

	return 0;
}
