#pragma once

#include <iostream>
#include <sstream>

#include <zmq.hpp>

#include <boost/bind.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
//#include <boost/thread/thread.hpp> 

#include "server.hxx"

server::server (void)
{
	init ();
}

server::~server (void)
{
	fin ();
}

const bool server::init (void)
{
	reception = nullptr;
	_running = false;
	
	//context_publisher = new zmq::context_t (1);
	//context_subscriber = new zmq::context_t (1);
	context = new zmq::context_t (1);
	
	//socket_publisher = new zmq::socket_t (*context_publisher, ZMQ_PUB);
	//socket_subscriber = new zmq::socket_t (*context_subscriber, ZMQ_SUB);
	socket = new zmq::socket_t (*context, ZMQ_ROUTER);
	//socket = new zmq::socket_t (*context, ZMQ_DEALER);
	
	//socket_subscriber -> setsockopt (ZMQ_SUBSCRIBE, "", 0);
	
	//socket_publisher -> bind ("tcp://*:2131");
	//socket_subscriber -> connect ("tcp://0.0.0.0:2132");
	socket -> bind ("tcp://*:2131");
	//socket -> connect ("tcp://0.0.0.0:2132");
	
	return true;
}

const bool server::fin (void)
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

const bool server::transmit (const std::string & message, const bool & more) const
{
	std::cout << "server::transmit()" << std::endl;
	
	//  Prepare our context and publisher
	//zmq::context_t context (1);
	//zmq::socket_t socket (context, ZMQ_PUB);
	zmq::message_t zmq_message (message.size ());
	
	//socket.bind ("tcp://*:2131");
	
	//snprintf ((char *) zmq_message.data (), strlen (message.c_str ()) + 1, "%s", message.c_str ());
	//snprintf ((char *) zmq_message.data (), strlen (message.c_str ()) + 1, "%s", message.c_str ());
	memcpy (zmq_message.data (), message.data (), message.size ());
	//std::cout << "server::transmit()::zmq_message==[" << static_cast <const char *> (zmq_message.data ()) << ']' << std::endl;
	
	
	//if (!socket_publisher -> send (zmq_message))
	if (!socket -> send (zmq_message, more ? ZMQ_SNDMORE : 0))
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
	
	//zmq::context_t context (1);
	//zmq::socket_t socket (context, ZMQ_SUB);
	zmq::message_t zmq_message;
	
	//socket_subscriber -> connect ("tcp://0.0.0.0:2132");
	
	////if (socket.setsockopt (ZMQ_SUBSCRIBE, filter, strlen (filter)); != 0)
	//if (socket.setsockopt (ZMQ_SUBSCRIBE, filter, strlen (filter)); != 0)
	//	return false;
	//socket.setsockopt (ZMQ_SUBSCRIBE, "", 0);
	
	//if (!socket_subscriber -> recv (&zmq_message))
	if (!socket -> recv (&zmq_message))
	{
		std::cout << "server::receive()::socket.recv()==[false]" << std::endl;
		return false;
	}
	std::cout << "server::receive()::socket.recv()==[true]" << std::endl;
	//std::cout << "server::receive()::zmq_message==[" << static_cast <const char *> (zmq_message.data ()) << ']' << std::endl;
	//message = static_cast <const char *> (zmq_message.data ());
	message = std::string (static_cast <const char *> (zmq_message.data ()), zmq_message.size ());
//message = std::istringstream (static_cast <char *> (zmq_message.data ())).str ();
	std::cout << "server::receive()::message==[" << message << ']' << std::endl;
	
	return true;
}

void server::_receive (void)
{
	std::cout << "server::_receive()" << std::endl;
	
	std::string message;
	std::string identity;
	std::string query;
	noware::array <noware::array <>> result;
	noware::array <std::string, int> arguments;
	// Work-around
	std::string size;
	std::istringstream iss;
	
	while (_running)
	{
		result.clear ();
		arguments.clear ();
		
		std::cout << "server::_receive()::loop" << std::endl;
		
		
		// Receive the identity.
		std::cout << "server::_receive(identity)" << std::endl;
		if (!receive (identity))
		{
			std::cout << "server::_receive()::receive(identity)==[false]" << std::endl;
			
			// Try again.
			continue;
		}
		std::cout << "server::_receive()::receive(identity)==[" << identity << ']' << std::endl;
		
		std::cout << "server::_receive()::receive(1)" << std::endl;
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
					if (!receive (message))
					{
						std::cout << "server::_receive()::receive(id)==[false]" << std::endl;
						
						// Abort.
						continue;
					}
					std::cout << "server::_receive()::receive(id)==[true]" << std::endl;
					std::cout << "server::_receive()::receive(id)::message==[" << message << ']' << std::endl;
					
					//std::cout << "server::_receive()::receive(name)::message==[id]" << std::endl;
					
					// We use limit here, because IDs may be non-unique,
					// so we can get more than one.
					query = "select uid, gid, username, 'x' as \"password\", shell, homedir, gecos from passwd where username = ? limit 1";
					arguments [1] = message;
					std::cout << "server::_receive()::arguments[1]==[" << arguments [1] << ']' << std::endl;
					
					// Send (like a 'set' operation) the peer identity first (who to send to).
					std::cout << "server::_receive()::transmit(identity)" << std::endl;
					if (!transmit (identity, true))
					{
						std::cout << "server::_receive()::transmit(identity)==[false]" << std::endl;
						
						// Abort.
						continue;
					}
					std::cout << "server::_receive()::transmit(identity)==[true]" << std::endl;
					
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
						// Nothing was found.
						if (result.empty ())
						{
							std::cout << "server::_receive()::result.empty()==[true]" << std::endl;
							
							// Announce that we have not found anything by failing the request.
							transmit ("0");
							
							// Abort this request.
							continue;
						}
						
						// Send the success status (succeeded).
						if (!transmit ("1", true))
						{
							std::cout << "server::_receive()::transmit(1)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// The order matters.
						
						// UID:
						std::cout << "server::_receive()::result[1][1](UID)==[" << result [1] [1] << "]" << std::endl;
						if (!transmit (result [1] [1], true))
						{
							std::cout << "server::_receive()::transmit(result[1])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// GID:
						std::cout << "server::_receive()::result[1][2](GID)==[" << result [1] [2] << "]" << std::endl;
						if (!transmit (result [1] [2], true))
						{
							std::cout << "server::_receive()::transmit(result[2])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Name
						std::cout << "server::_receive()::result[1][3](user name)==[" << result [1] [3] << "]" << std::endl;
						if (!transmit (result [1] [3], true))
						{
							std::cout << "server::_receive()::transmit(result[3])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Password:
						std::cout << "server::_receive()::result[1][4](password)==[" << result [1] [4] << "]" << std::endl;
						if (!transmit (result [1] [4], true))
						{
							std::cout << "server::_receive()::transmit(result[4])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Shell:
						std::cout << "server::_receive()::result[1][5](shell)==[" << result [1] [5] << "]" << std::endl;
						if (!transmit (result [1] [5], true))
						{
							std::cout << "server::_receive()::transmit(result[5])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Home:
						std::cout << "server::_receive()::result[1][6](home)==[" << result [1] [6] << "]" << std::endl;
						if (!transmit (result [1] [6], true))
						{
							std::cout << "server::_receive()::transmit(result[6])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// GECOS:
						std::cout << "server::_receive()::result[1][7](gecos)==[" << result [1] [7] << "]" << std::endl;
						if (!transmit (result [1] [7]))
						{
							std::cout << "server::_receive()::transmit(result[7])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
					}
				}
				else if (message == "id")
				{
					if (!receive (message))
					{
						std::cout << "server::_receive()::receive(id)==[false]" << std::endl;
						
						// Abort.
						continue;
					}
					std::cout << "server::_receive()::receive(id)==[true]" << std::endl;
					std::cout << "server::_receive()::receive(id)::message==[" << message << ']' << std::endl;
					
					//std::cout << "server::_receive()::receive(name)::message==[id]" << std::endl;
					
					// We use limit here, because IDs may be non-unique,
					// so we can get more than one.
					query = "select uid, gid, username, 'x' as \"password\", shell, homedir, gecos from passwd where uid = ? limit 1";
					arguments [1] = message;
					std::cout << "server::_receive()::arguments[1]==[" << arguments [1] << ']' << std::endl;
					
					// Send (like a 'set' operation) the peer identity first (who to send to).
					std::cout << "server::_receive()::transmit(identity)" << std::endl;
					if (!transmit (identity, true))
					{
						std::cout << "server::_receive()::transmit(identity)==[false]" << std::endl;
						
						// Abort.
						continue;
					}
					std::cout << "server::_receive()::transmit(identity)==[true]" << std::endl;
					
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
						// Nothing was found.
						if (result.empty ())
						{
							std::cout << "server::_receive()::result.empty()==[true]" << std::endl;
							
							// Announce that we have not found anything by failing the request.
							transmit ("0");
							
							// Abort this request.
							continue;
						}
						
						//boost::this_thread::sleep (boost::posix_time::milliseconds (500));
						
						// Send the success status (succeeded).
						if (!transmit ("1", true))
						{
							std::cout << "server::_receive()::transmit(1)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// The order matters.
						
						// UID:
						std::cout << "server::_receive()::result[1][1](UID)==[" << result [1] [1] << "]" << std::endl;
						if (!transmit (result [1] [1], true))
						{
							std::cout << "server::_receive()::transmit(result[1])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// GID:
						std::cout << "server::_receive()::result[1][2](GID)==[" << result [1] [2] << "]" << std::endl;
						if (!transmit (result [1] [2], true))
						{
							std::cout << "server::_receive()::transmit(result[2])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Name
						std::cout << "server::_receive()::result[1][3](user name)==[" << result [1] [3] << "]" << std::endl;
						if (!transmit (result [1] [3], true))
						{
							std::cout << "server::_receive()::transmit(result[3])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Password:
						std::cout << "server::_receive()::result[1][4](password)==[" << result [1] [4] << "]" << std::endl;
						if (!transmit (result [1] [4], true))
						{
							std::cout << "server::_receive()::transmit(result[4])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Shell:
						std::cout << "server::_receive()::result[1][5](shell)==[" << result [1] [5] << "]" << std::endl;
						if (!transmit (result [1] [5], true))
						{
							std::cout << "server::_receive()::transmit(result[5])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Home:
						std::cout << "server::_receive()::result[1][6](home)==[" << result [1] [6] << "]" << std::endl;
						if (!transmit (result [1] [6], true))
						{
							std::cout << "server::_receive()::transmit(result[6])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// GECOS:
						std::cout << "server::_receive()::result[1][7](gecos)==[" << result [1] [7] << "]" << std::endl;
						if (!transmit (result [1] [7]))
						{
							std::cout << "server::_receive()::transmit(result[7])==[false]" << std::endl;
							
							// Abort.
							continue;
						}
					}
				}
				
				result.clear ();
				arguments.clear ();
			}
			else if (message == "group")
			{
				std::cout << "server::_receive()::message==[group]" << std::endl;
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
					if (!receive (message))
					{
						std::cout << "server::_receive()::receive(id)==[false]" << std::endl;
						
						// Abort.
						continue;
					}
					std::cout << "server::_receive()::receive(id)==[true]" << std::endl;
					std::cout << "server::_receive()::receive(id)::message==[" << message << ']' << std::endl;
					
					//std::cout << "server::_receive()::receive(name)::message==[id]" << std::endl;
					
					// We use limit here, because IDs may be non-unique,
					// so we can get more than one.
					query = "select id, name, 'x' as \"password\" from groups where name = ? limit 1";
					arguments [1] = message;
					std::cout << "server::_receive()::arguments[1]==[" << arguments [1] << ']' << std::endl;
					
					// Send (like a 'set' operation) the peer identity first (who to send to).
					std::cout << "server::_receive()::transmit(identity)" << std::endl;
					if (!transmit (identity, true))
					{
						std::cout << "server::_receive()::transmit(identity)==[false]" << std::endl;
						
						// Abort.
						continue;
					}
					std::cout << "server::_receive()::transmit(identity)==[true]" << std::endl;
					
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
						// Nothing was found.
						if (result.empty ())
						{
							std::cout << "server::_receive()::result.empty()==[true]" << std::endl;
							
							// Announce that we have not found anything by failing the request.
							transmit ("0");
							
							// Abort this request.
							continue;
						}
						
						// Send the success status (succeeded).
						if (!transmit ("1", true))
						{
							std::cout << "server::_receive()::transmit(1)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// The order matters.
						
						// GID:
						std::cout << "server::_receive()::result[1][1](GID)==[" << result [1] [1] << "]" << std::endl;
						if (!transmit (result [1] [1], true))
						{
							std::cout << "server::_receive()::transmit(result[1])(GID)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Group Name:
						std::cout << "server::_receive()::result[1][2](group name)==[" << result [1] [2] << "]" << std::endl;
						if (!transmit (result [1] [2], true))
						{
							std::cout << "server::_receive()::transmit(result[2])(group name)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Password
						std::cout << "server::_receive()::result[1][3](password)==[" << result [1] [3] << "]" << std::endl;
						if (!transmit (result [1] [3]))
						{
							std::cout << "server::_receive()::transmit(result[3])(password)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
					}
				}
				else if (message == "id")
				{
					if (!receive (message))
					{
						std::cout << "server::_receive()::receive(id)==[false]" << std::endl;
						
						// Abort.
						continue;
					}
					std::cout << "server::_receive()::receive(id)==[true]" << std::endl;
					std::cout << "server::_receive()::receive(id)::message==[" << message << ']' << std::endl;
					
					//std::cout << "server::_receive()::receive(name)::message==[id]" << std::endl;
					
					// We use limit here, because IDs may be non-unique,
					// so we can get more than one.
					query = "select id, name, 'x' as \"password\" from groups where id = ? limit 1";
					arguments [1] = message;
					std::cout << "server::_receive()::arguments[1]==[" << arguments [1] << ']' << std::endl;
					
					// Send (like a 'set' operation) the peer identity first (who to send to).
					std::cout << "server::_receive()::transmit(identity)" << std::endl;
					if (!transmit (identity, true))
					{
						std::cout << "server::_receive()::transmit(identity)==[false]" << std::endl;
						
						// Abort.
						continue;
					}
					std::cout << "server::_receive()::transmit(identity)==[true]" << std::endl;
					
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
						// Nothing was found.
						if (result.empty ())
						{
							std::cout << "server::_receive()::result.empty()==[true]" << std::endl;
							
							// Announce that we have not found anything by failing the request.
							transmit ("0");
							
							// Abort this request.
							continue;
						}
						
						// Send the success status (succeeded).
						if (!transmit ("1", true))
						{
							std::cout << "server::_receive()::transmit(1)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// The order matters.
						
						// GID:
						std::cout << "server::_receive()::result[1][1](GID)==[" << result [1] [1] << "]" << std::endl;
						if (!transmit (result [1] [1], true))
						{
							std::cout << "server::_receive()::transmit(result[1])(GID)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Group Name:
						std::cout << "server::_receive()::result[1][2](group name)==[" << result [1] [2] << "]" << std::endl;
						if (!transmit (result [1] [2], true))
						{
							std::cout << "server::_receive()::transmit(result[2])(group name)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						
						// Password
						std::cout << "server::_receive()::result[1][3](password)==[" << result [1] [3] << "]" << std::endl;
						if (!transmit (result [1] [3]))
						{
							std::cout << "server::_receive()::transmit(result[3])(password)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
					}
				}
				
				result.clear ();
				arguments.clear ();
			}
			else if (message == "groups")
			{
				std::cout << "server::_receive()::message==[group]" << std::endl;
				if (!receive (message))
				{
					std::cout << "server::_receive()::receive(user)==[false]" << std::endl;
					
					// Abort.
					continue;
				}
				std::cout << "server::_receive()::receive(user)==[true]" << std::endl;
				std::cout << "server::_receive()::receive(user)::message==[" << message << ']' << std::endl;
				
				if (message == "user_name")
				{
					if (!receive (message))
					{
						std::cout << "server::_receive()::receive(id)==[false]" << std::endl;
						
						// Abort.
						continue;
					}
					std::cout << "server::_receive()::receive(id)==[true]" << std::endl;
					std::cout << "server::_receive()::receive(id)::message==[" << message << ']' << std::endl;
					
					//std::cout << "server::_receive()::receive(name)::message==[id]" << std::endl;
					
					// We use limit here, because IDs may be non-unique,
					// so we can get more than one.
					query = "select user_group.gid from user_group join passwd on user_group.uid = passwd.uid where passwd.username = ? and passwd.gid != user_group.gid";
					arguments [1] = message;
					std::cout << "server::_receive()::arguments[1]==[" << arguments [1] << ']' << std::endl;
					
					// Send (like a 'set' operation) the peer identity first (who to send to).
					std::cout << "server::_receive()::transmit(identity)" << std::endl;
					if (!transmit (identity, true))
					{
						std::cout << "server::_receive()::transmit(identity)==[false]" << std::endl;
						
						// Abort.
						continue;
					}
					std::cout << "server::_receive()::transmit(identity)==[true]" << std::endl;
					
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
						std::cout << "server::groups::_receive()::transmit(1)" << std::endl;
						if (!transmit ("1", true))
						{
							std::cout << "server::groups::_receive()::transmit(1)==[false]" << std::endl;
							continue;
						}
						std::cout << "server::groups::_receive()::transmit(1)==[true]" << std::endl;
						
						//iss.clear ();
						//iss.str (result.size ().string ());
						//size = iss.str ();
						std::cout << "(result.size () == \"2\")==[" << (result.size () == "2") << ']' << std::endl;
						std::cout << "(result.size ().string () == \"2\")==[" << (result.size ().string () == "2") << ']' << std::endl;
						std::cout << "server::groups::_receive()::transmit(result.size ()==[" << result.size () << "])" << std::endl;
						std::cout << "server::groups::_receive()::transmit(result.size ().string ()==[" << result.size ().string () << "])" << std::endl;
						if (!transmit (result.size ().string (), true))
						{
							std::cout << "server::groups::_receive()::transmit(result.size==[" << result.size () << "])==[false]" << std::endl;
							continue;
						}
						std::cout << "server::groups::_receive()::transmit(result.size==[" << result.size () << "])==[true]" << std::endl;
						
						std::cout << "server::groups::_receive()::transmit::loop" << std::endl;
						//for (const noware::array <std::string, int>::container & argument : arguments)
						for (const 	noware::array <noware::array <>>::container & row : result)
						{
							std::cout << "server::groups::_receive()::transmit(" << row.v [1] << ")" << std::endl;
							if (!transmit (row.v [1/* column */], true))
							{
								std::cout << "server::groups::_receive()::transmit(" << row.v [1] << ")==[false]" << std::endl;
								continue;
							}
							std::cout << "server::groups::_receive()::transmit(" << row.v [1] << ")==[true]" << std::endl;
						}
						std::cout << "server::groups::_receive()::transmit::loop::done" << std::endl;
						
						std::cout << "server::groups::_receive()::transmit(end)" << std::endl;
						if (!transmit ("end"))
						{
							std::cout << "server::groups::_receive()::transmit(end)==[false]" << std::endl;
							continue;
						}
						std::cout << "server::groups::_receive()::transmit(end)==[true]" << std::endl;
					}
				}
				
				//result.clear ();
				//arguments.clear ();
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
	
	if (!db.connect ("../cfg/system.db"))
		return false;
	
	if (reception == nullptr)
		reception = new boost::thread (boost::bind (boost::mem_fn (&server::_receive), this));
	
	_running = true;
	
	return true;
}
