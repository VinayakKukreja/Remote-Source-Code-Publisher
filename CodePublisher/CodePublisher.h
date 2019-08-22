#pragma once

////////////////////////////////////////////////////////////////////////////////
//  CodePublisher.h - Functionality to convert and publish files matching     //
//					  specific patterns and regex. Also supports GUI calls    //
//  ver 1.2																	  //
//  Language:      Visual C++, Visual Studio 2017							  //
//  Platform:      Dell XPS 8920, Windows 10								  //
//  Application:   Prototype for CSE687 - OOD Projects						  //
//  Source:        Jim Fawcett, CST 4-187, Syracuse University				  //
//                 (315) 443-3948, jfawcett@twcny.rr.com					  //
//  Author:		   Vinayak Kukreja, Syracuse University						  //
////////////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package provides functionality to convert and publish files matching specific patterns and regex. Also supports GUI calls
*
* Public Interface:
* -----------------
*  Class CodePublisher:

	CodePublisher();
		--> Default Constructor

	bool processCommandLineArgs(int argc, char ** argv);
		--> process cmd to get info

	void workingDirectory(const std::string& dir);
		--> return input directory - from PCL

	const std::string& workingDirectory() const;
		--> return input directory - from PCL

	void outputDirectory(const std::string& dir);
		--> set output directory

	const std::string& outputDirectory() const;
		--> return output directory

	bool extractFiles();
		--> extract files - after processing cmd

	bool extractAllFiles();
		--> extract files - after processing cmd

	std::vector<std::string> getFilteredFiles();
		--> Returns the files that matched the pattern and regexes

	std::vector<std::string> getAllFiles();
		--> Returns all the files that matches the file extensions

	void publish();
		--> publish - files extracted from directory explorer

	void publish(const std::string& file);
		--> publish - single file

	void publish(const std::vector<std::string>& files);
		--> publishes a list of files

	Utilities::DisplayMode displayMode() const;
		--> gets display mode as set by PCL

	ICodePublisher* Objectfactory::createPublisherInstance()
		--> Returns new instance of CodePublisher as a ICodePublisher i.e. interface pointer

	std::vector<std::string> CodePublisher::GUIPublish(int argc, char** argv)
		--> Function used by the GUI via the interface to use the CodePublisher functionality

*
*  Required Files:
* =======================
*  Utilities.h Utilities.cpp
*  Toker.h Toker.cpp
*  ActionsAndRules.h ActionsAndRules.cpp
*  Semi.h Semi.cpp
*  CodePublisher.h CodePublisher.cpp
*  ICodePublisher.h ICodePublisher.cpp
*  FileSystem.h FileSystem.cpp
*  DirExplorerN.h DirExplorerN.cpp
*  DependencyTable.h DependencyTable.cpp
*  Logger.h Logger.cpp
*  Converter.h Converter.cpp
*  DependencyAnalyzer.h DependencyAnalyzer.cpp
*  CodeUtilities.h CodeUtilities.cpp
*  Display.h Display.cpp
*
*  Maintainence History:
* =======================
*  ver 1.0 - 11 Feb 2019
*  - Source
*  - first release
*  ver 1.1 - 14 March 2019
*  - Author
*  - updated release
*  ver 1.2 - 10 April 2019
*  - Author
*  - Added GUI publishing functionality
*/


#include <string>
#include <vector>

#include "../Converter/Converter.h"
#include "../Display/Display.h"
#include "../CodeUtilities/CodeUtilities.h"
#include "ICodePublisher.h"

class CodePublisher : public ICodePublisher
{
public:
	CodePublisher();
	bool processCommandLineArgs(int argc, char ** argv);
	void workingDirectory(const std::string& dir);
	const std::string& workingDirectory() const;
	void outputDirectory(const std::string& dir);
	const std::string& outputDirectory() const;
	bool extractFiles();
	bool extractAllFiles();
	std::vector<std::string> getFilteredFiles();
	std::vector<std::string> getAllFiles();
	void publish();
	void publish(const std::string& file);
	void publish(const std::vector<std::string>& files);
	std::vector<std::string> GUIPublish(int argc, char ** argv);
	std::vector<std::string> displayFiles(std::vector<std::string>);
	UtilitiesCodeUtilities::DisplayMode displayMode() const;

private:
	UtilitiesCodeUtilities::ProcessCmdLine *pcl_;
	CodeConverter cconv_;
	Display display_;
	std::string dirIn_;
	std::string dirOut_;
	std::string relativePath_;
	std::vector<std::string> files_;
	std::vector<std::string> allfiles_;
};
