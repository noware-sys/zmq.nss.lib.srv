#pragma once

#include <iostream>

#include <zmq.hpp>

#include <boost/bind.hpp>

#include "client.hxx"

client::client (void)
{
	reception = nullptr;
	_running = false;
}

client::~client (void)
{
	stop ();
}

const bool client::transmit (const std::string & message) const
{
	std::cout << "client::transmit()" << std::endl;
	
	//  Prepare our context and publisher
	zmq::context_t context (1);
	zmq::socket_t socket (context, ZMQ_PUB);
	zmq::message_t zmq_message (message.length ());
	
	socket.bind ("tcp://127.0.0.1:2131");
	
	snprintf ((char *) zmq_message.data (), message.length (), "%s", message);
	
	if (socket.send (zmq_message) != 0)
	{
		std::cout << "client::transmit()::socket.send()==[false]" << std::endl;
		return false;
	}
	
	std::cout << "client::transmit()::socket.send()==[true]" << std::endl;
	return true;
}

const bool client::receive (std::string & message) const
{
	std::cout << "client::receive()" << std::endl;
	
	zmq::context_t context (1);
	zmq::socket_t socket (context, ZMQ_SUB);
	zmq::message_t zmq_message;
	
	socket.connect ("tcp://127.0.0.1:2132");
	
	////if (socket.setsockopt (ZMQ_SUBSCRIBE, filter, strlen (filter)); != 0)
	//if (socket.setsockopt (ZMQ_SUBSCRIBE, filter, strlen (filter)); != 0)
	//	return false;
	
	if (socket.recv (&zmq_message) != 0)
	{
		std::cout << "client::receive()::socket.recv()==[false]" << std::endl;
		return false;
	}
	message = (char *) zmq_message.data ();
	std::cout << "client::receive()::socket.recv()==[true]" << std::endl;
	std::cout << "client::receive()::message==[" << message << ']' << std::endl;
	
	return true;
}

void client::_receive (void)
{
	std::cout << "client::_receive()" << std::endl;
	
	std::string message;
	std::string query;
	noware::array <noware::array <>> result;
	noware::array <noware::var, int> arguments;
	
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
	
	if (!db.connect ("/root/Projects/exo.nss.lib/cfg/system-test.db"))
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
	if (!transmit ("get"))
		return false;
	if (!transmit ("user"))
		return false;
	if (!transmit ("name"))
		return false;
	if (!transmit ("testuser"))
		return false;
	
	// Get the success status:
	if (!receive (message))
		return false;
	
	if (message == "0")
		return false;
	std::cout << "client::tx_passwd()::received::success==[true]" << std::endl;
	
	// Get the user ID (as a character pointer):
	if (!receive (message))
		return false;
	std::cout << "client::tx_passwd()::received::uid==[" << message << "]" << std::endl;
	
	// Get the primary group ID (as a character pointer):
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
	
	// Get the home:
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
