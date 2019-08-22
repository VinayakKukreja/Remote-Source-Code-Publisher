////////////////////////////////////////////////////////////////////////////////
//  CodePublisher.cpp - Functionality to convert and publish files matching   //
//						specific patterns and regex. Also supports GUI calls  //
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

#include <iostream>
#include <string>
#include "../UtilitiesCodePublisher/UtilitiesCodePublisher.h"
#include "../Tokenizer/Toker.h"
#include "../SemiExpression/Semi.h"
#include "Parser.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"
#include "CodePublisher.h"
#include "../DependencyAnalyzer/DependencyAnalyzer.h"
#include "../DirExplorer-Naive/DirExplorerN.h"


//using namespace Utilities;
using namespace Logging;
using namespace FileSystem;

using namespace Lexer;
using namespace UtilitiesCodeUtilities;
using Demo = StaticLogger<1>;

#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include <queue>
#include <string>
#define Util StringHelper

using namespace CodeAnalysis;

// -----< Default Constructor >------------------------------------
CodePublisher::CodePublisher()
{

}

// -----< Returns new instance of CodePublisher as a ICodePublisher i.e. interface pointer >------------------------------------
ICodePublisher* Objectfactory::createPublisherInstance()
{
	std::cout << "\t\t\t--> Returning a new CodePublisher instance" << std::endl;
	return new CodePublisher;
}


ProcessCmdLine::Usage customUsageEnteries();

// -----< process cmd to get info >------------------------------------
bool CodePublisher::processCommandLineArgs(int argc, char ** argv)
{
	pcl_ = new ProcessCmdLine(argc, argv);
	if (pcl_->parseError())
	{
		pcl_->usage();
		std::cout << "\n\n";
		return false;
	}
	dirIn_ = pcl_->path();
	return true;
}

// -----< return input directory - from PCL >----------------------------
const std::string & CodePublisher::workingDirectory() const
{
	return dirIn_;
}

// -----< set output directory >-----------------------------------------
void CodePublisher::outputDirectory(const std::string & dir)
{
	dirOut_ = dir;
	cconv_.outputDir(dir);
}

// -----< return output directory >--------------------------------------
const std::string & CodePublisher::outputDirectory() const
{
	return dirOut_;
}

// -----< extract files - after processing cmd >-------------------------
bool CodePublisher::extractFiles()
{
	DirExplorerN de(pcl_->path());
	std::string relativePath = pcl_->path();
	LoggerDemo::write("\n\nThe path is : " + Path::getFullFileSpec(relativePath) + " and the current directory is : " + FileSystem::Directory::getCurrentDirectory() + "\n\n");
	for (auto patt : pcl_->patterns())
	{
		de.addPattern(patt);
	}
	if (pcl_->hasOption("s"))
	{
		de.recurse();
	}
	bool res = de.search() && de.match_regexes(pcl_->regexes());
	de.showStats();
	files_ = de.filesList();
	allfiles_ = de.returnAllMatchingFiles();
	return res;
}

// -----< extract files - after processing cmd >-------------------------
bool CodePublisher::extractAllFiles()
{
	if (allfiles_.size() > 0)
	{
		return true;
	}
	return false;
}

// -----< Returns the files that matched the pattern and regexes >-------------------------
std::vector<std::string> CodePublisher::getFilteredFiles()
{
	return files_;
}

// -----< Returns all the files that matches the file extensions >-------------------------
std::vector<std::string> CodePublisher::getAllFiles()
{
	return allfiles_;
}

// -----< publish - files extracted from directory explorer >---------------
void CodePublisher::publish()
{
	display_.display(cconv_.convert(files_));
}

// -----< publish - single file >-------------------------------------------
void CodePublisher::publish(const std::string & file)
{
	display_.display(cconv_.convert(file));
}

// -----< publishes a list of files >-------------------------------------
void CodePublisher::publish(const std::vector<std::string>& files)
{
	display_.display(files);
}

// -----< gets display mode as set by PCL >---------------------------------
UtilitiesCodeUtilities::DisplayMode CodePublisher::displayMode() const
{
	return pcl_->displayMode();
}

// -----< command line usage >----------------------------------------------
ProcessCmdLine::Usage customUsageEnteries()
{
	std::string usage;
	usage += "\n  Command Line: path [/option]* [/pattern]* [/regex]*";
	usage += "\n    path is relative or absolute path where processing begins";
	usage += "\n    [/option]* are one or more options of the form:";
	usage += "\n      /s     - walk directory recursively";
	usage += "\n      /demo  - run in demonstration mode (cannot coexist with /debug)";
	usage += "\n      /debug - run in debug mode (cannot coexist with /demo)";
	//usage += "\n      /h - hide empty directories";
	//usage += "\n      /a - on stopping, show all files in current directory";
	usage += "\n    [pattern]* are one or more pattern strings of the form:";
	usage += "\n      *.h *.cpp *.cs *.txt or *.*";
	usage += "\n    [regex] regular expression(s), i.e. [A-B](.*)";
	usage += "\n";
	return usage;
}

// -----< initialize loggers in proper display mode >------------------------
void initializeLoggers(DisplayMode dm) {
	if (dm == DisplayMode::Debug)
	{
		LoggerDebug::attach(&std::cout);
		LoggerDebug::start();
	}
	else if (dm == DisplayMode::Demo)
	{
		LoggerDemo::attach(&std::cout);
		LoggerDemo::start();
	}
}

// -----< stop loggers >-----------------------------------------------------
void stopLoggers() {
	LoggerDemo::stop();
	LoggerDebug::stop();
}

// -----< Function used by the GUI via the interface to use the CodePublisher functionality >-----------------------------------------------------
std::vector<std::string> CodePublisher::GUIPublish(int argc, char** argv)
{
	std::cout << "\n\t\t--> In CodePublisher's GUIPublish function" << std::endl;
	CodePublisher exe;
	DependencyAnalyzer dependencyAnalysis;
	DependencyTable dependencyTable;
	std::vector<std::string> all_file_names;
	std::vector<std::string> test_file_names;
	std::cout << "\t\t\t--> Trying to process received command line arguments" << std::endl;
	if (!exe.processCommandLineArgs(argc, argv)) return {};
	std::cout << "\t\t\t\t--> Processed Successfully" << std::endl;
	std::cout << "\t\t\t--> Extracting files satisfying command line arguments" << std::endl;
	if (!exe.extractFiles())
	{
		std::cout << ("\t\t\t\t--> No files found in the current directory") << std::endl;
	}
	else
	{
		test_file_names = exe.getFilteredFiles();
		std::cout << "\t\t\t\t--> Extracted Successfully" << std::endl;
	}

	if (!exe.extractAllFiles())
	{
		std::cout << ("\t\t\t\t--> No files found in the current directory") << std::endl;
	}
	else
	{
		all_file_names = exe.getAllFiles();
	}
	std::cout << "\t\t\t--> Trying to find the dependencies between files and constructing the dependency table" << std::endl;
	dependencyTable = dependencyAnalysis.analyzeDependenciesGUICall(test_file_names, all_file_names);
	std::cout << "\t\t\t\t--> Dependency Table constructed" << std::endl;
	std::cout << "\t\t\t--> Initializing the CodeConverted with the dependency table" << std::endl;
	CodeConverter codeConverter(dependencyTable);
	codeConverter.clearConvertedFilesFolder();
	std::cout << "\t\t\t\t--> Trying to convert the files" << std::endl;
	codeConverter.convert();
	std::cout << "\t\t\t\t\t--> Files converted successfully" << std::endl;
	std::cout << "\t\t\t--> Retrieving the converted files" << std::endl;
	std::vector<std::string> convertedFiles = codeConverter.convertedFiles();
	std::cout << "\t\t\t\t--> Retrieved, now returning a vector of strings having the converted files" << std::endl;
	return convertedFiles;
}

#ifdef TEST_CODE

// -----< This is the test-stub for CodePublisher >-----------------------------------------------------
int main(int argc, char* argv[])
{
	CodePublisher exe;
	DependencyAnalyzer dependencyAnalysis;
	DependencyTable dependencyTable;
	std::vector<std::string> all_file_names;
	std::vector<std::string> test_file_names;
	UtilitiesParser::StringHelper::Title("Testing Project2 CodePublisher");
	UtilitiesParser::putline();
	initializeLoggers(UtilitiesCodeUtilities::DisplayMode::Debug);
	for (int i = 0; i < argc; i++)
	//{
	//	std::string arg = argv[i];
	//	if (arg == "/demo") {
	//		stopLoggers();
	//		initializeLoggers(UtilitiesCodeUtilities::DisplayMode::Demo);
	//		break;
	//	}
	//}
	if (!exe.processCommandLineArgs(argc, argv)) return 1;

	if (!exe.extractFiles())
	{
		std::cout << ("\n\n  No files found in the current directory");
	}
	else
	{
		test_file_names = exe.getFilteredFiles();
	}

	if (!exe.extractAllFiles())
	{
		std::cout << ("\n\n  No files found in the current directory");
	}
	else
	{
		all_file_names = exe.getAllFiles();
	}
	dependencyTable = dependencyAnalysis.analyzeDependencies(test_file_names, all_file_names);
	dependencyTable.display();
	CodeConverter codeConverter(dependencyTable);
	codeConverter.outputDir("..\\ConvertedWebpages\\");
	codeConverter.convert();
	std::vector<std::string> convertedFiles = codeConverter.convertedFiles();
	exe.publish(codeConverter.convertedFiles());
	std::cout << "\n\n\n\n\n\n-->  Testing GUIPublish function using mock arguments\n" << std::endl;
	exe.GUIPublish(argc, argv);
}

#endif