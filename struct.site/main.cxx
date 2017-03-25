//#pragma once

//#include <Poco/any.h>
#include <string>
#include <iostream>

#include <server>

int main (const int argc, const char * argv [])
{
	//using boost::any_cast;
	//using namespace std;
	//using namespace noware;
	
	for (int i = 0; i < argc; ++i)
		std::cout << argv [i] << ' ';
	std::cout << std::endl;
	
	std::cout << "Argument count: " << argc << std::endl;
	
	server srv;
	std::string selection;
	
	if (srv.start ())
		std::cout << "Server started & running..." << std::endl;
	else
		std::cout << "Server startup failed (not running)..." << std::endl;
	
	std::getline (std::cin, selection);
}
