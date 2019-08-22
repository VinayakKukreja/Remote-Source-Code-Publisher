////////////////////////////////////////////////////////////////////////////////////////
// DependencyAnalyzer.cpp   : Functionality to find dependencies between given files //
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
*  Logger.h Logger.cpp FileSystem.h FileSystem.cpp Parser.h Parser.cpp
*  ConfigureParser.h ConfigureParser.cpp DirExplorerN.h DirExplorerN.cpp
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

#include "DependencyAnalyzer.h"
#include "../DirExplorer-Naive/DirExplorerN.h"
#include "../Parser/Parser.h"
#include "../Parser/ConfigureParser.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../DependencyTable/DependencyTable.h"
#include <algorithm>

// -----< Default constructor >--------------------------------------------------
DependencyAnalyzer::DependencyAnalyzer()
{
}

// -----< Destructor >--------------------------------------------------
DependencyAnalyzer::~DependencyAnalyzer()
{
}

// -----< To process command line >--------------------------------------------------
bool DependencyAnalyzer::processCommandLineArgs(int argc, char ** argv)
{
	pcl_ = new UtilitiesCodeUtilities::ProcessCmdLine(argc, argv);
	if (pcl_->parseError())
	{
		pcl_->usage();
		std::cout << "\n\n";
		return false;
	}
	dirIn_ = pcl_->path();
	return true;
}

// -----< extract files - after processing cmd >-------------------------
bool DependencyAnalyzer::extractFiles()
{
	FileSystem::DirExplorerN de(pcl_->path());
	std::string relativePath = pcl_->path();
	Logging::LoggerDemo::write("\n\nThe path is : " + FileSystem::Path::getFullFileSpec(relativePath) + " and the current directory is : " + FileSystem::Directory::getCurrentDirectory() + "\n\n");

	std::cout << "\n\nThe path is : " + FileSystem::Path::getFullFileSpec(relativePath) + " and the current directory is : " + FileSystem::Directory::getCurrentDirectory() + "\n\n";

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
bool DependencyAnalyzer::extractAllFiles()
{
	if (allfiles_.size() > 0)
	{
		return true;
	}
	return false;
}

// -----< Functionality to analyze dependencies and checking if file exists >--------------------------------------------------
DependencyTable DependencyAnalyzer::analyzeDependencies(std::vector<std::string> testFiles, std::vector<std::string> allFiles)
{
	DependencyTable dependencyTable;
	fileToAnalyze = testFiles;
	allFilesExisting = allFiles;
	for (auto file : fileToAnalyze)
	{
		std::string fileSpec = file;
		std::string fileName = FileSystem::Path::getName(fileSpec);
		CodeAnalysis::ConfigParseForCodeAnal configure;
		CodeAnalysis::Parser* pParser = configure.Build();
		try
		{
			if (pParser)
			{
				if (!configure.Attach(fileSpec))
				{
					std::cout << "\n  could not open file " << fileName << std::endl;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
			}
			CodeAnalysis::Repository* pRepo = CodeAnalysis::Repository::getInstance();
			pRepo->package() = fileName;
			while (pParser->next())
				pParser->parse();
			dependencyTable = getDependencies(pRepo, dependencyTable, file);
		}
		catch (std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
			std::cout << "\n  exception caught at line " << __LINE__ << " ";
		}
	}
	return dependencyTable;
}

// -----< Functionality to getDependencies of a file and make respective entries in dependency table >--------------------------------------------------
DependencyTable DependencyAnalyzer::getDependencies(CodeAnalysis::Repository* pRepo, DependencyTable dependencyTable, std::string file)
{
	int count = 0;
	CodeAnalysis::ASTNode* pGlobalScope = pRepo->getGlobalScope();
	auto iter2 = pGlobalScope->statements_.begin();
	while (iter2 != pGlobalScope->statements_.end())
	{
		std::string temp = (*iter2)->ToString();
		int index = temp.find('\"');
		if (index != -1)
		{
			std::string value = "";
			std::string tempFilePath = "";
			std::string finalPath = "";
			int indexFile;
			std::string currentDirectory = FileSystem::Directory::getCurrentDirectory();
			std::string tempPartialPath = temp.substr(index);
			tempPartialPath.erase(std::remove(tempPartialPath.begin(), tempPartialPath.end(), '\"'), tempPartialPath.end());
			indexFile = tempPartialPath.find_last_of('/');
			if (indexFile != -1)
			{
				value = tempPartialPath.substr(indexFile + 1);
				tempFilePath = currentDirectory + "/" + tempPartialPath;
				finalPath = FileSystem::Path::getFullFileSpec(tempFilePath);
			}
			else
			{
				value = tempPartialPath;
				std::string filePath = FileSystem::Path::getPath(file);
				tempFilePath = filePath + tempPartialPath;
				finalPath = FileSystem::Path::getFullFileSpec(tempFilePath);
			}
			if (std::find(allFilesExisting.begin(), allFilesExisting.end(), finalPath) != allFilesExisting.end())
			{
				count += 1;
				dependencyTable.addDependency(file, finalPath);
			}
		}
		++iter2;
	}
	if (count == 0)
	{
		dependencyTable.addDependency(file, "");
	}
	return dependencyTable;
}

// -----< Functionality to analyze dependencies and checking if file exists when call made from GUI >--------------------------------------------------
DependencyTable DependencyAnalyzer::analyzeDependenciesGUICall(std::vector<std::string> testFiles, std::vector<std::string> allFiles)
{
	DependencyTable dependencyTable;
	fileToAnalyze = testFiles;
	allFilesExisting = allFiles;
	for (auto file : fileToAnalyze)
	{
		std::string fileSpec = file;
		std::string fileName = FileSystem::Path::getName(fileSpec);
		CodeAnalysis::ConfigParseForCodeAnal configure;
		CodeAnalysis::Parser* pParser = configure.Build();
		try
		{
			if (pParser)
			{
				if (!configure.Attach(fileSpec))
				{
					std::cout << "\n  could not open file " << fileName << std::endl;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
			}
			CodeAnalysis::Repository* pRepo = CodeAnalysis::Repository::getInstance();
			pRepo->package() = fileName;
			while (pParser->next())
				pParser->parse();
			dependencyTable = getDependenciesGUICall(pRepo, dependencyTable, file);
		}
		catch (std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
			std::cout << "\n  exception caught at line " << __LINE__ << " ";
		}
	}
	return dependencyTable;
}

// -----< Functionality to getDependencies of a file and make respective entries in dependency table  when call made from GUI >--------------------------------------------------
DependencyTable DependencyAnalyzer::getDependenciesGUICall(CodeAnalysis::Repository* pRepo, DependencyTable dependencyTable, std::string file)
{
	int count = 0;
	CodeAnalysis::ASTNode* pGlobalScope = pRepo->getGlobalScope();
	auto iter2 = pGlobalScope->statements_.begin();
	while (iter2 != pGlobalScope->statements_.end())
	{
		std::string temp = (*iter2)->ToString();
		int index = temp.find('\"');
		if (index != -1)
		{
			std::string value = "";
			std::string tempFilePath = "";
			std::string finalPath = "";
			int indexFile;
			std::string currentDirectory = FileSystem::Directory::getCurrentDirectory();
			std::string tempPartialPath = temp.substr(index);
			tempPartialPath.erase(std::remove(tempPartialPath.begin(), tempPartialPath.end(), '\"'), tempPartialPath.end());
			indexFile = tempPartialPath.find_last_of('/');
			if (indexFile != -1)
			{
				value = tempPartialPath.substr(indexFile + 1);
				tempFilePath = currentDirectory + "../" + tempPartialPath;
				finalPath = FileSystem::Path::getFullFileSpec(tempFilePath);
			}
			else
			{
				value = tempPartialPath;
				std::string filePath = FileSystem::Path::getPath(file);
				tempFilePath = filePath + tempPartialPath;
				finalPath = FileSystem::Path::getFullFileSpec(tempFilePath);
			}
			if (std::find(allFilesExisting.begin(), allFilesExisting.end(), finalPath) != allFilesExisting.end())
			{
				count += 1;
				dependencyTable.addDependency(file, finalPath);
			}
		}
		++iter2;
	}
	if (count == 0)
	{
		dependencyTable.addDependency(file, "");
	}
	return dependencyTable;
}

#ifdef TEST_DEPEND
int main(int argc, char ** argv)
{
	DependencyAnalyzer dependencyAnalyzer;
	DependencyTable dependencyTable;
	std::vector<std::string> all_files;
	std::vector<std::string> required_files;
	dependencyAnalyzer.processCommandLineArgs(argc, argv);

	if (dependencyAnalyzer.extractFiles())
	{
		required_files = dependencyAnalyzer.returnFilteredFiles();
	}
	else
	{
		std::cout << "Something Went Wrong!!" << std::endl;
	}

	if (dependencyAnalyzer.extractAllFiles())
	{
		all_files = dependencyAnalyzer.returnAllFiles();
	}
	else
	{
		std::cout << "Something Went Wrong!!" << std::endl;
	}

	dependencyTable = dependencyAnalyzer.analyzeDependencies(required_files, all_files);
	dependencyTable.coutdisplay();
	std::cout << std::endl;
}
#endif