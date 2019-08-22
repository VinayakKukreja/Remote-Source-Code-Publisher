#pragma once
//////////////////////////////////////////////////////////////////////////
// Server.h -		Console App that processes incoming messages,		//
//					a mock server channel for client					//
// ver 1.1																//
// Application   : Acts as a server for our GUI clients					//
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64     //
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018    //
// Author: Vinayak Kukreja, Syracuse University, vkukreja@syr.edu		//
//////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* ---------------------
*  Package contains one class, Server, that contains a Message-Passing Communication
*  facility. It processes each message by invoking an installed callable object
*  defined by the message's command key.
*
*  Message handling runs on a child thread, so the Server main thread is free to do
*  any necessary background processing (none, so far).
*
*  It also provides the client with the functionality to convert source files to html
*  and receive them if they want to.
*
** Public Interface:
* -----------------
*  Class Server:

		Server(MsgPassingCommunication::EndPoint ep, const std::string& name);
			--> Initialize server endpoint and give server a name

		void start();
			--> start server's instance of Comm

		void stop();
			--> Stop Comm instance

		void addMsgProc(Key key, ServerProc proc);
			--> Add ServerProc callable object to server's dispatcher

		void processMessages();
			--> Start processing messages on child thread

		void postMessage(MsgPassingCommunication::Message msg);
			--> Pass message to Comm for sending

		MsgPassingCommunication::Message getMessage();
			--> Gets message from Comm

		static Dirs getDirs(const SearchPath& path = storageRoot);
			--> Wrapper around Directory::getDirectories

		static Files getFiles(const SearchPath& path = storageRoot, const std::string& pattern = "*.*");
			--> Wrapper around Directory::getFiles

*  Required Files:
* -----------------
*  Server.h, Server.cpp
*  Comm.h, Comm.cpp, IComm.h
*  Sockets.h Sockets.cpp
*  CodePublisher.h CodePublisher.cpp
*  Message.h, Message.cpp
*  FileSystem.h, FileSystem.cpp
*  Utilities.h
*
*  Maintenance History:
* ----------------------
*  ver 1.0 : 3/27/2018
*  - first release
*  - Source
*  ver 1.1 : 05/01/2019
*  - Second release
*  - Author
*  - Added conversion of source to html files functionality
*/

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <thread>
#include "../Message/Message.h"
#include "../MsgPassingComm/Comm.h"
#include <windows.h>
#include <tchar.h>

namespace Repository
{
	using File = std::string;
	using Files = std::vector<File>;
	using Dir = std::string;
	using Dirs = std::vector<Dir>;
	using SearchPath = std::string;
	using Key = std::string;
	using Msg = MsgPassingCommunication::Message;
	using ServerProc = std::function<Msg(Msg)>;
	using MsgDispatcher = std::unordered_map<Key, ServerProc>;

	const SearchPath storageRoot = "../Storage";  // root for all server file storage
	MsgPassingCommunication::EndPoint serverEndPoint("localhost", 8080);  // listening endpoint

	//----< Server class >----------
	class Server
	{
	public:
		Server(MsgPassingCommunication::EndPoint ep, const std::string& name);
		void start();
		void stop();
		void addMsgProc(Key key, ServerProc proc);
		void processMessages();
		void postMessage(MsgPassingCommunication::Message msg);
		MsgPassingCommunication::Message getMessage();
		static Dirs getDirs(const SearchPath& path = storageRoot);
		static Files getFiles(const SearchPath& path = storageRoot, const std::string& pattern = "*.*");
	private:
		MsgPassingCommunication::Comm comm_;
		MsgDispatcher dispatcher_;
		std::thread msgProcThrd_;
	};

	//----< Initialize server endpoint and give server a name >----------
	inline Server::Server(MsgPassingCommunication::EndPoint ep, const std::string& name)
		: comm_(ep, name) {}

	//----< Start server's instance of Comm >----------------------------
	inline void Server::start()
	{
		comm_.start();
	}

	//----< Stop Comm instance >-----------------------------------------
	inline void Server::stop()
	{
		if (msgProcThrd_.joinable())
			msgProcThrd_.join();
		comm_.stop();
	}

	//----< Pass message to Comm for sending >---------------------------
	inline void Server::postMessage(MsgPassingCommunication::Message msg)
	{
		comm_.postMessage(msg);
	}

	//----< Get message from Comm >--------------------------------------
	inline MsgPassingCommunication::Message Server::getMessage()
	{
		Msg msg = comm_.getMessage();
		return msg;
	}
	
	//----< Add ServerProc callable object to server's dispatcher >------
	inline void Server::addMsgProc(Key key, ServerProc proc)
	{
		dispatcher_[key] = proc;
	}

	//----< Start processing messages on child thread >------------------
	inline void Server::processMessages()
	{
		auto proc = [&]()
		{
			if (dispatcher_.size() == 0)
			{
				std::cout << "\n  no server procs to call";
				return;
			}
			while (true)
			{
				Msg msg = getMessage();
				std::cout << "\n**********************************************************************************************************************************\n" << std::endl;
				std::cout << "\n\t      Received message: " << msg.command() << " from " << msg.from().toString() << std::endl;
				if (msg.containsKey("verbose"))
				{
					std::cout << "\n";
					std::cout << "\n--------------------------------------------------------------------------------------------------" << std::endl;
					msg.show();
					std::cout << "\n--------------------------------------------------------------------------------------------------" << std::endl;
				}
				if (msg.command() == "serverQuit")
					break;
				Msg reply = dispatcher_[msg.command()](msg);
				if (msg.to().port != msg.from().port)  // avoid infinite message loop
				{
					postMessage(reply);
					std::cout << "\n--------------------------------------------------------------------------------------------------" << std::endl;
					msg.show();
					reply.show();
					std::cout << "\n--------------------------------------------------------------------------------------------------" << std::endl;
				}
				else
					std::cout << "\n\t      Server attempting to post to self" << std::endl;
			}
			std::cout << "\n\t      Server message processing thread is shutting down" << std::endl;
			std::cout << "\n**********************************************************************************************************************************\n" << std::endl;
		};
		std::thread t(proc);
		std::cout << "\n\t      Starting server thread to process messages" << std::endl;
		msgProcThrd_ = std::move(t);
	}
}