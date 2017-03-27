#pragma once

#include <string>

#include <boost/function.hpp>
//#include <boost/function_equal.hpp>
#include <boost/thread.hpp>

#include <noware/db/sqlite>
#include <noware/array>

class client
{
	public:
		client (void);
		~client (void);
		
		const bool stop (void);
		const bool start (void);
		const bool status (void) const;
		
		const bool transmit (const std::string &) const;
		const bool receive (std::string &) const;
		
		// Simulate requests of [passwd], [group], [shadow].
		const bool tx_passwd (void) const;
		const bool tx_group (void) const;
		const bool tx_shadow (void) const;
	protected:
		boost::thread * reception;	// For running "void receive (void)".
		
		// Manager of incoming messages.
		void _receive (void);
		
		bool _running;
		
		zmq::context_t * context_publisher;
		zmq::context_t * context_subscriber;
		
		zmq::socket_t * socket_publisher;
		zmq::socket_t * socket_subscriber;
		
		noware::db::sqlite db;
};
