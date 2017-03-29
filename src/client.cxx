#pragma once

#include <iostream>

#include <zmq.hpp>
//#include <zmq.h>
//#include <zmq/zhelpers.hxx>

#include <boost/bind.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
//#include <boost/thread/thread.hpp> 

#include "client.hxx"

client::client (void)
{
	init ();
}

client::~client (void)
{
	fin ();
}

const bool client::init (void)
{
	reception = nullptr;
	_running = false;
	
	//context_publisher = new zmq::context_t (1);
	//context_subscriber = new zmq::context_t (1);
	context = new zmq::context_t (1);
	
	//socket_publisher = new zmq::socket_t (*context_publisher, ZMQ_PUB);
	//socket_subscriber = new zmq::socket_t (*context_subscriber, ZMQ_SUB);
	socket = new zmq::socket_t (*context, ZMQ_DEALER);
	
	//socket_subscriber -> setsockopt (ZMQ_SUBSCRIBE, "", 0);
	
	//socket_publisher -> bind ("tcp://*:2132");
	//socket_subscriber -> connect ("tcp://0.0.0.0:2131");
	//socket -> bind ("tcp://*:2132");
	socket -> connect ("tcp://0.0.0.0:2131");
	
	return true;
}

const bool client::fin (void)
{
	stop ();
	
	//delete socket_subscriber;
	//delete socket_publisher;
	delete socket;
	
	//delete context_subscriber;
	//delete context_publisher;
	delete context;
	
	return true;
}

const bool client::transmit (const std::string & message, const bool & more) const
{
	std::cout << "client::transmit()" << std::endl;
	
	//zmq::context_t * context_publisher = new zmq::context_t (1);
	//zmq::socket_t * socket_publisher = new zmq::socket_t (*context_publisher, ZMQ_PUB);
	//socket_publisher -> bind ("tcp://*:2132");
	

	//  Prepare our context and publisher
	//zmq::context_t context (1);
	//zmq::socket_t socket (context, ZMQ_PUB);
	zmq::message_t zmq_message (message.size ());
	
	//socket_publisher -> bind ("tcp://*:2132");
	
	//snprintf ((char *) zmq_message.data (), strlen (message.c_str ()) + 1, "%s", message.c_str ());
	memcpy (zmq_message.data (), message.data (), message.size ());
	//std::cout << "client::transmit()::zmq_message==[" << static_cast <const char *> (zmq_message.data ()) << ']' << std::endl;
	
	//if (!socket_publisher -> send (zmq_message))
	if (!socket -> send (zmq_message, more ? ZMQ_SNDMORE : 0))
	{
		//delete socket_publisher;
		//delete context_publisher;
		
		std::cout << "client::transmit()::socket.send()==[false]" << std::endl;
		return false;
	}
	
	//delete socket_publisher;
	//delete context_publisher;
	
	std::cout << "client::transmit()::socket.send()==[true]" << std::endl;
	return true;
}

const bool client::receive (std::string & message) const
{
	std::cout << "client::receive()" << std::endl;
	
	//zmq::context_t * context_subscriber = new zmq::context_t (1);
	//zmq::socket_t * socket_subscriber = new zmq::socket_t (*context_subscriber, ZMQ_SUB);
	//socket_subscriber -> setsockopt (ZMQ_SUBSCRIBE, "", 0);
	//socket_subscriber -> connect ("tcp://0.0.0.0:2131");
	
	
	//zmq::context_t context (1);
	//zmq::socket_t socket (context, ZMQ_SUB);
	zmq::message_t zmq_message;
	
	//socket.connect ("tcp://0.0.0.0:2131");
	
	////if (socket.setsockopt (ZMQ_SUBSCRIBE, filter, strlen (filter)); != 0)
	//if (socket.setsockopt (ZMQ_SUBSCRIBE, filter, strlen (filter)); != 0)
	//	return false;
	//socket.setsockopt (ZMQ_SUBSCRIBE, "", 0);
	
	//if (!socket_subscriber -> recv (&zmq_message))
	if (!socket -> recv (&zmq_message))
	{
		//delete socket_subscriber;
		//delete context_subscriber;
		
		std::cout << "client::receive()::socket.recv()==[false]" << std::endl;
		return false;
	}
	std::cout << "client::receive()::socket.recv()==[true]" << std::endl;
	//std::cout << "client::receive()::zmq_message==[" << static_cast <const char *> (zmq_message.data ()) << ']' << std::endl;
	//message = static_cast <const char *> (zmq_message.data ());
	message = std::string (static_cast <const char *> (zmq_message.data ()), zmq_message.size ());
	std::cout << "client::receive()::message==[" << message << ']' << std::endl;
	
	//delete socket_subscriber;
	//delete context_subscriber;
	
	return true;
}

void client::_receive (void)
{
	std::cout << "client::_receive()" << std::endl;
	
	std::string message;
	std::string query;
	noware::array <noware::array <>> result;
	noware::array <std::string, int> arguments;
	
	while (_running)
	{
		std::cout << "client::_receive()::loop" << std::endl;
		
		if (!receive (message))
		{
			std::cout << "client::_receive()::receive(1)==[false]" << std::endl;
			
			// Try again.
			continue;
		}
		std::cout << "client::_receive()::receive(1)::message==[" << message << ']' << std::endl;
		
		if (message == "get")
		{
			if (!receive (message))
			{
				std::cout << "client::_receive()::receive(get)==[false]" << std::endl;
				
				// Abort.
				continue;
			}
			std::cout << "client::_receive()::receive(get)==[true]" << std::endl;
			std::cout << "client::_receive()::receive(get)::message==[" << message << ']' << std::endl;
			
			if (message == "user")
			{
				if (!receive (message))
				{
					std::cout << "client::_receive()::receive(user)==[false]" << std::endl;
					
					// Abort.
					continue;
				}
				std::cout << "client::_receive()::receive(user)==[true]" << std::endl;
				std::cout << "client::_receive()::receive(user)::message==[" << message << ']' << std::endl;
				
				if (message == "name")
				{
					std::cout << "client::_receive()::receive(name)::message==[name]" << std::endl;
					
					// We do not use limit here, because user names should not be non-unique (they should be unique).
					query = "select uid, gid, username, 'x' as \"password\", shell, home, gecos from passwd where username = ?1";
					arguments [1] = message;
				}
				else if (message == "id")
				{
					std::cout << "client::_receive()::receive(name)::message==[id]" << std::endl;
					
					// We use limit here, because IDs may be non-unique,
					// so we can get more than one.
					query = "select uid, gid, username, 'x' as \"password\", shell, home, gecos from passwd where uid = ?1 limi 1";
					arguments [1] = message;
					
					if (!db.query (result, query, arguments))
					{
						std::cout << "client::_receive()::db.query()==[false]" << std::endl;
						// Send the success status (failed),
						// so the application would not continue requesting in the same session.
						if (!transmit ("0"))
						{
							std::cout << "client::_receive()::transmit(0)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
					}
					else
					{
						// Send the success status (succeeded).
						if (!transmit ("1"))
						{
							std::cout << "client::_receive()::transmit(1)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// The order matters.
						
						// UID:
						if (!transmit (result [1]))
						{
							std::cout << "client::_receive()::transmit(result[1])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// GID:
						if (!transmit (result [2]))
						{
							std::cout << "client::_receive()::transmit(result[2])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Name
						if (!transmit (result [3]))
						{
							std::cout << "client::_receive()::transmit(result[3])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Password:
						if (!transmit (result [4]))
						{
							std::cout << "client::_receive()::transmit(result[4])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Shell:
						if (!transmit (result [5]))
						{
							std::cout << "client::_receive()::transmit(result[5])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Home:
						if (!transmit (result [6]))
						{
							std::cout << "client::_receive()::transmit(result[6])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// GECOS:
						if (!transmit (result [7]))
						{
							std::cout << "client::_receive()::transmit(result[7])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
					}
				}
			}
			else if (message == "group")
			{
				std::cout << "client::_receive()::message==[group]" << std::endl;
			}
		}
		
		std::cout << "client::_receive()::loop::end" << std::endl;
	}
}

const bool client::status (void) const
{
	return _running;
}

const bool client::stop (void)
{
	if (!status ())
		return true;
	
	if (reception != nullptr)
	{
		delete reception;
		reception = nullptr;
	}
	
	if (!db.disconnect ())
		return false;
	
	_running = false;
	
	return true;
}

const bool client::start (void)
{
	if (status ())
		return true;
	
	if (!db.connect ("../cfg/system.db"))
		return false;
	
	if (reception == nullptr)
		reception = new boost::thread (boost::bind (boost::mem_fn (&client::_receive), this));
	
	_running = true;
	
	return true;
}

const bool client::tx_passwd (void) const
{
	// Response.
	std::string message;
	
	
	// Send the request (in parts):
	if (!transmit ("get", true))
		return false;
	if (!transmit ("user", true))
		return false;
	if (!transmit ("id", true))
		return false;
	if (!transmit ("1005"))
		return false;
	
	//zclock_sleep (500);
	//boost::this_thread::sleep (boost::posix_time::milliseconds (500));
	
	// Get the success status:
	if (!receive (message))
		return false;
	
	if (message == "0")
		return false;
	std::cout << "client::tx_passwd()::received::success==[true]" << std::endl;
	
	// Get the user ID:
	if (!receive (message))
		return false;
	std::cout << "client::tx_passwd()::received::uid==[" << message << "]" << std::endl;
	
	// Get the primary group ID:
	if (!receive (message))
		return false;
	std::cout << "client::tx_passwd()::received::gid==[" << message << "]" << std::endl;
	
	// Get the name:
	if (!receive (message))
		return false;
	std::cout << "client::tx_passwd()::received::username==[" << message << "]" << std::endl;
	
	// Get the password:
	if (!receive (message))
		return false;
	std::cout << "client::tx_passwd()::received::password==[" << message << "]" << std::endl;
	
	// Get the shell:
	if (!receive (message))
		return false;
	std::cout << "client::tx_passwd()::received::shell==[" << message << "]" << std::endl;
	
	// Get the home path:
	if (!receive (message))
		return false;
	std::cout << "client::tx_passwd()::received::home==[" << message << "]" << std::endl;
	
	// Get the GECOS field:
	if (!receive (message))
		return false;
	std::cout << "client::tx_passwd()::received::gecos==[" << message << "]" << std::endl;
	
	return true;
}

const bool client::tx_group (void) const
{
	return false;
}

const bool client::tx_shadow (void) const
{
	return false;
}
