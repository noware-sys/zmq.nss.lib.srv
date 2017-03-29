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
		
		const bool fin (void);
		const bool init (void);
		
		const bool stop (void);
		const bool start (void);
		const bool status (void) const;
		
		const bool transmit (const std::string &, const bool &/* more*/ = false) const;
		const bool receive (std::string &) const;
	protected:
		boost::thread * reception;	// For running "void receive (void)".
		
		// Manager of incoming messages.
		void _receive (void);
		
		bool _running;
		
		//zmq::context_t * context_publisher;
		//zmq::context_t * context_subscriber;
		zmq::context_t * context;
		
		//zmq::socket_t * socket_publisher;
		//zmq::socket_t * socket_subscriber;
		zmq::socket_t * socket;
		
		noware::db::sqlite db;
};
