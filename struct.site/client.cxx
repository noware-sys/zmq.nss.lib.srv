//#pragma once

//#include <Poco/any.h>
#include <string>
#include <iostream>

#include <client>

int main (const int argc, const char * argv [])
{
	//using boost::any_cast;
	//using namespace std;
	//using namespace noware;
	
	for (int i = 0; i < argc; ++i)
		std::cout << argv [i] << ' ';
	std::cout << std::endl;
	
	std::cout << "Argument count: " << argc << std::endl;
	
	client c;
	std::string selection;
	
	if (c.start ())
		std::cout << "Client started & running..." << std::endl;
	else
	{
		std::cout << "Client startup failed (not running)..." << std::endl;
		
		return EXIT_FAILURE;
	}
	
	std::cout << "Press [Enter] to send [passwd] request > " << std::endl;
	std::getline (std::cin, selection);
	c.tx_passwd ();
}
