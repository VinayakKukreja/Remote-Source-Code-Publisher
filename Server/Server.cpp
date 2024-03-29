//////////////////////////////////////////////////////////////////////////
// Server.cpp - Console App that processes incoming messages,			//
//					   a mock server channel for client					//
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

#include "Server.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include <chrono>
#include <iterator>
#include "../CodePublisher/CodePublisher.h"


namespace MsgPassComm = MsgPassingCommunication;
using namespace Repository;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;

//----< Wrapper around Directory::getFiles >----------
Files Server::getFiles(const Repository::SearchPath& path, const std::string& pattern)
{
	return Directory::getFiles(path, pattern);
}

//----< Wrapper around Directory::getDirectories >----------
Dirs Server::getDirs(const Repository::SearchPath& path)
{
	return Directory::getDirectories(path);
}

//----< To display the message contents >----------
template<typename T>
void show(const T& t, const std::string& msg)
{
	std::cout << "\n  " << msg.c_str();
	for (auto item : t)
	{
		std::cout << "\n    " << item.c_str();
	}
}

//----< Functionality to take place when requested command is "echo" >----------
std::function<Msg(Msg)> echo = [](Msg msg) {
	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	return reply;
};

//----< Functionality to take place when requested command is "clientConnectionRequest", establishes if connection is made >----------
std::function<Msg(Msg)> clientConnectionRequest = [](Msg msg) {
	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("serverConnectionRequest");
	return reply;
};

//----< Functionality to take place when requested command is "getConvertedFiles", sends list of successfully converted files back to the client >----------
std::function<Msg(Msg)> getConvertedFiles = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("setConvertedFilesList");
	std::string args = msg.value("cmdArgs");
	std::istringstream convertToVector(args);
	std::vector<std::string> arguments{
	std::istream_iterator<std::string>{convertToVector},
	std::istream_iterator<std::string>{} };
	std::vector<char*> argv;
	for (const auto& arg : arguments)
		argv.push_back((char*)arg.data());
	argv.push_back(nullptr);
	CodePublisher codePublisher;
	auto convertedFilesList = codePublisher.GUIPublish(argv.size() - 1, argv.data());
	size_t count = 0;
	for (auto convertedFiles : convertedFilesList)
	{
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("convertedFile" + countStr, convertedFiles);
	}
	return reply;
};

//----< Functionality to take place when requested command is "transferConvertedFiles", transfers the requested file to the client >----------
std::function<Msg(Msg)> transferConvertedFiles = [](Msg msg)
{
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.value("file");
	reply.command("receiveConvertedFile");
	reply.file(msg.value("file"));
	return reply;
};

//----< Functionality to take place when requested command is "getFiles", sends list of files matching patterns at a specified server path >----------
std::function<Msg(Msg)> getFiles = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getFiles");
	std::string path = msg.value("path");
	std::string patterns = msg.value("patterns");
	std::string text = patterns;
	std::istringstream iss(text);
	std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
		std::istream_iterator<std::string>());
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != ".")
			searchPath = searchPath + "\\" + path;
		Files files;
		Files combined;
		for (auto patt : results)
		{
			files = Server::getFiles(searchPath, patt);
			for (auto file : files)
			{
				combined.push_back(file);
			}
		}
		size_t count = 0;
		for (auto item : combined)
		{
			std::string countStr = Utilities::Converter<size_t>::toString(++count);
			reply.attribute("file" + countStr, item);
		}
	}
	else
	{
		std::cout << "\n  getFiles message did not define a path attribute";
	}
	return reply;
};

//----< Functionality to take place when requested command is "getDirs", sends list of directories at a specified server path >----------
std::function<Msg(Msg)> getDirs = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getDirs");
	std::string path = msg.value("path");
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != ".")
			searchPath = searchPath + "\\" + path;
		Files dirs = Server::getDirs(searchPath);
		size_t count = 0;
		for (auto item : dirs)
		{
			if (item != ".." && item != ".")
			{
				std::string countStr = Utilities::Converter<size_t>::toString(++count);
				reply.attribute("dir" + countStr, item);
			}
		}
	}
	else
	{
		std::cout << "\n  getDirs message did not define a path attribute";
	}
	return reply;
};

//----< Execution starts here for this file >----------
int main(int argc, char* argv[])
{
	std::cout << "\n ==========================";
	std::cout << "\n\t  Server";
	std::cout << "\n ==========================";
	std::cout << "\n";
	std::vector<std::string> connection;
	std::string s = argv[1];
	std::string delimiter = ":";
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		connection.push_back(s.substr(0, pos));
		std::cout << token << std::endl;
		s.erase(0, pos + delimiter.length());
	}
	connection.push_back(s);
	serverEndPoint.address = connection[0];
	serverEndPoint.port = std::stoi(connection[1]);
	Server server(serverEndPoint, "ServerPrototype");
	server.start();
	server.addMsgProc("echo", echo);
	server.addMsgProc("getFiles", getFiles);
	server.addMsgProc("getDirs", getDirs);
	server.addMsgProc("getConvertedFiles", getConvertedFiles);
	server.addMsgProc("transferConvertedFiles", transferConvertedFiles);
	server.addMsgProc("serverQuit", echo);
	server.addMsgProc("clientConnectionRequest", clientConnectionRequest);
	server.processMessages();
	std::cout << "\n";
	std::cin.get();
	return 0;
}

