#pragma once

#include <string>

#include <boost/function.hpp>
//#include <boost/function_equal.hpp>
#include <boost/thread.hpp>

#include <noware/db/sqlite>
#include <noware/array>

class server
{
	public:
		server (void);
		~server (void);
		
		const bool stop (void);
		const bool start (void);
		const bool status (void) const;
		
		const bool transmit (const std::string &) const;
		const bool receive (std::string &) const;
	protected:
		boost::thread * reception;	// For running "void receive (void)".
		
		// Manager of incoming messages.
		void _receive (void);
		
		bool _running;
		
		
		noware::db::sqlite db;
};
