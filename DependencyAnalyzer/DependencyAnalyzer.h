#pragma once

////////////////////////////////////////////////////////////////////////////////////////
// DependencyAnalyzer.h   : Functionality to find dependencies between given files   //
// ver 1.2																		    //
//																				   // 
// Application   : Analyzes depenendencies between files						  //
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64				 //
// Author		 : Vinayak Kukreja, Syracuse University						    //
/////////////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package helps in finding out dependencies between files and adds the result
*  to a dependency table structure which is an unordered map with keys as the files that
*  are analyzed and values as the vector of files that it is dependent upon
*  Also supports GUI call now
*
* Public Interface:
* -----------------
*  Class DependencyAnlayzer:

		DependencyAnalyzer();
			--> Default constructor

		~DependencyAnalyzer();
			--> Destructor

		DependencyTable analyzeDependencies(std::vector<std::string>, std::vector<std::string>);
			--> Functionality to analyze dependencies and checking if file exists

		DependencyTable getDependencies(CodeAnalysis::Repository*, DependencyTable, std::string);
			--> Functionality to getDependencies of a file and make respective entries in dependency table

		DependencyTable analyzeDependenciesGUICall(std::vector<std::string>, std::vector<std::string>);
			--> Functionality to analyze dependencies and checking if file exists when call made from GUI
			--> Different from above due to relative path

		DependencyTable getDependenciesGUICall(CodeAnalysis::Repository*, DependencyTable, std::string);
			--> Functionality to getDependencies of a file and make respective entries in dependency table  when call made from GUI
			--> Different from above due to relative path

		bool extractFiles();
			--> extract files - after processing cmd

		bool extractAllFiles();
			--> extract files - after processing cmd

		std::vector<std::string> returnFilteredFiles() { return files_; }
			--> Returns list of files which matched the regex patterns

		std::vector<std::string> returnAllFiles() { return allfiles_; }
			--> Returns all files matching patterns

		bool processCommandLineArgs(int argc, char ** argv);
			--> To process command line
*
*  Required Files:
* =======================
*  DependencyAnalyzer.h DependencyAnalyzer.cpp DependencyTable.h DependencyTable.cpp
*  Logger.h Logger.cpp FileSystem.h FileSystem.cpp
*
*  Maintainence History:
* =======================
*  ver 1.1 - 14 March 2019
*  - Author
*  - first release
*  ver 1.2 - 10 April 2019
*  -Author
*  -Resolved relative path bug while finding dependencies
*/

#include "../CodeUtilities/CodeUtilities.h"
#include "../Display/Display.h"
#include "../DependencyTable/DependencyTable.h"


namespace CodeAnalysis {
	class Repository;
}

class DependencyAnalyzer
{
private:
	std::vector<std::string> fileToAnalyze;
	std::vector<std::string> allFilesExisting;
	UtilitiesCodeUtilities::ProcessCmdLine *pcl_;
	std::string dirIn_;
	std::vector<std::string> files_;
	std::vector<std::string> allfiles_;
public:
	DependencyAnalyzer();
	~DependencyAnalyzer();
	bool extractFiles();
	bool extractAllFiles();
	std::vector<std::string> returnFilteredFiles() { return files_; }
	std::vector<std::string> returnAllFiles() { return allfiles_; }
	DependencyTable analyzeDependencies(std::vector<std::string>, std::vector<std::string>);
	DependencyTable getDependencies(CodeAnalysis::Repository*, DependencyTable, std::string);
	DependencyTable analyzeDependenciesGUICall(std::vector<std::string>, std::vector<std::string>);
	DependencyTable getDependenciesGUICall(CodeAnalysis::Repository*, DependencyTable, std::string);
	bool processCommandLineArgs(int argc, char ** argv);
};

