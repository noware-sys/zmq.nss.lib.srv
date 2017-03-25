#pragma once

#include <iostream>

#include <zmq.hpp>

#include <boost/bind.hpp>

#include "server.hxx"

server::server (void)
{
	reception = nullptr;
	_running = false;
}

server::~server (void)
{
	stop ();
}

const bool server::transmit (const std::string & message) const
{
	std::cout << "server::transmit()" << std::endl;
	
	//  Prepare our context and publisher
	zmq::context_t context (1);
	zmq::socket_t socket (context, ZMQ_PUB);
	zmq::message_t zmq_message;
	
	socket.bind ("tcp://127.0.0.1:2132");
	
	snprintf ((char *) zmq_message.data (), message.length (), "%s", message);
	
	if (socket.send (zmq_message) != 0)
	{
		std::cout << "server::transmit()::socket.send()==[false]" << std::endl;
		return false;
	}
	
	std::cout << "server::transmit()::socket.send()==[true]" << std::endl;
	return true;
}

const bool server::receive (std::string & message) const
{
	std::cout << "server::receive()" << std::endl;
	
	zmq::context_t context (1);
	zmq::socket_t socket (context, ZMQ_SUB);
	zmq::message_t zmq_message;
	
	socket.connect ("tcp://127.0.0.1:2131");
	
	////if (socket.setsockopt (ZMQ_SUBSCRIBE, filter, strlen (filter)); != 0)
	//if (socket.setsockopt (ZMQ_SUBSCRIBE, filter, strlen (filter)); != 0)
	//	return false;
	
	if (socket.recv (&zmq_message) != 0)
	{
		std::cout << "server::receive()::socket.recv()==[false]" << std::endl;
		return false;
	}
	message = (char *) zmq_message.data ();
	std::cout << "server::receive()::socket.recv()==[true]" << std::endl;
	std::cout << "server::receive()::message==[" << message << ']' << std::endl;
	
	return true;
}

void server::_receive (void)
{
	std::cout << "server::_receive()" << std::endl;
	
	std::string message;
	std::string query;
	noware::array <noware::array <>> result;
	noware::array <noware::var, int> arguments;
	
	while (_running)
	{
		std::cout << "server::_receive()::loop" << std::endl;
		
		if (!receive (message))
		{
			std::cout << "server::_receive()::receive(1)==[false]" << std::endl;
			
			// Try again.
			continue;
		}
		std::cout << "server::_receive()::receive(1)::message==[" << message << ']' << std::endl;
		
		if (message == "get")
		{
			if (!receive (message))
			{
				std::cout << "server::_receive()::receive(get)==[false]" << std::endl;
				
				// Abort.
				continue;
			}
			std::cout << "server::_receive()::receive(get)==[true]" << std::endl;
			std::cout << "server::_receive()::receive(get)::message==[" << message << ']' << std::endl;
			
			if (message == "user")
			{
				if (!receive (message))
				{
					std::cout << "server::_receive()::receive(user)==[false]" << std::endl;
					
					// Abort.
					continue;
				}
				std::cout << "server::_receive()::receive(user)==[true]" << std::endl;
				std::cout << "server::_receive()::receive(user)::message==[" << message << ']' << std::endl;
				
				if (message == "name")
				{
					std::cout << "server::_receive()::receive(name)::message==[name]" << std::endl;
					
					// We do not use limit here, because user names should not be non-unique (they should be unique).
					query = "select uid, gid, username, 'x' as \"password\", shell, home, gecos from passwd where username = ?1";
					arguments [1] = message;
				}
				else if (message == "id")
				{
					std::cout << "server::_receive()::receive(name)::message==[id]" << std::endl;
					
					// We use limit here, because IDs may be non-unique,
					// so we can get more than one.
					query = "select uid, gid, username, 'x' as \"password\", shell, home, gecos from passwd where uid = ?1 limi 1";
					arguments [1] = message;
					
					if (!db.query (result, query, arguments))
					{
						std::cout << "server::_receive()::db.query()==[false]" << std::endl;
						// Send the success status (failed),
						// so the application would not continue requesting in the same session.
						if (!transmit ("0"))
						{
							std::cout << "server::_receive()::transmit(0)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
					}
					else
					{
						// Send the success status (succeeded).
						if (!transmit ("1"))
						{
							std::cout << "server::_receive()::transmit(1)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// The order matters.
						
						// UID:
						if (!transmit (result [1]))
						{
							std::cout << "server::_receive()::transmit(result[1])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// GID:
						if (!transmit (result [2]))
						{
							std::cout << "server::_receive()::transmit(result[2])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Name
						if (!transmit (result [3]))
						{
							std::cout << "server::_receive()::transmit(result[3])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Password:
						if (!transmit (result [4]))
						{
							std::cout << "server::_receive()::transmit(result[4])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Shell:
						if (!transmit (result [5]))
						{
							std::cout << "server::_receive()::transmit(result[5])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Home:
						if (!transmit (result [6]))
						{
							std::cout << "server::_receive()::transmit(result[6])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// GECOS:
						if (!transmit (result [7]))
						{
							std::cout << "server::_receive()::transmit(result[7])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
					}
				}
			}
			else if (message == "group")
			{
				std::cout << "server::_receive()::message==[group]" << std::endl;
			}
		}
		
		std::cout << "server::_receive()::loop::end" << std::endl;
	}
}

const bool server::status (void) const
{
	return _running;
}

const bool server::stop (void)
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

const bool server::start (void)
{
	if (status ())
		return true;
	
	if (!db.connect ("/root/Projects/exo.nss.lib/cfg/system-test.db"))
		return false;
	
	if (reception == nullptr)
		reception = new boost::thread (boost::bind (boost::mem_fn (&server::_receive), this));
	
	_running = true;
	
	return true;
}
