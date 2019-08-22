#pragma once

///////////////////////////////////////////////////////////////////////////
// Converter.h   : Defines source code conversion to webpage functions   //
// ver 1.1                                                               //
//                                                                       // 
// Application   : Converts given source files to HTML files             //
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64      //
// Source        : Ammar Salman, Syracuse University                     //
//                 313/788-4694, assalman@syr.edu						 //
// Author		 : Vinayak Kukreja, Syracuse University                  //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines CodeConverter class which accepts DependencyTable
*  and uses its relationships to create linked webpages that point to each
*  other based on dependencies. It can also convert single files if needed.
*  The conversion process filters HTML special characters before printing
*  them into output files. The resulting output of this converter is a list
*  of files (vector<string>) of the created webpages.
*
* Public Interface:
* -----------------
*  Class CodeConverter:

		CodeConverter();
			--> Default Constructor

		CodeConverter(const DependencyTable& dt);
			--> Constructor accepting dependency table

		const std::string& outputDir() const;
			--> Returns outputDir_ i.e. output directory relative path

		void outputDir(const std::string& dir);
			--> Sets outputDir_

		bool createOutpurDir();
			--> Creates output directory

		void setDepTable(const DependencyTable& dt);
			--> Sets Dependency Table value

		std::vector<std::string> convert();
			--> Converts the files in dependency table

		std::string convert(const std::string& filepath);
			--> Converts single file given path

		std::vector<std::string> convert(const std::vector<std::string>& files);
			--> Sets dependency table to files and converts

		void convertListOfFiles(const std::vector<std::string>& files);
			--> Converts a list of files provided

		std::string insertSingleToggleButton(int);
			--> Inserts a HTML button at a given position with specific id

		std::string insertDivSingleLineComment(int);
			--> Inserts a HTML div with Comment as the class at a given position with specific id

		int clearConvertedFilesFolder();
			--> To clear the contents of ConvertedWebpages folder before creating new converted web pages

		std::string insertDivMultiLineComment(int);
			--> Inserts a HTML div with Comment as the class at a given position with specific id

		std::string insertDivClass(int);
			--> Inserts a HTML div with Classes as the class at a given position with specific id

		std::string insertDivFunction(int);
			--> Inserts a HTML div with Functions as the class at a given position with specific id

		std::string insertEndingDiv();
			--> Inserts a HTML ending div i.e. </div>

		int setDivId();
			--> Increases the counter to add as unique id for requiring HTML elements

		void addAllToggleButtons();
			--> To add buttons that can toggle all specific elements such as functions, comments, classes, at a  specific position

		void createUniqueListAndParse();
			--> Creates a new list with unique elements with the help of uniqueListElements()

		void handleSingleLineComments(std::string& line);
			--> Functionality to detect and handle comments i.e. adding divs at specific positions in a file

		bool handleMultiLineComments(std::string& line);
			--> Functionality to detect and handle multi-line comments i.e. adding divs at specific positions in a file

		void handleMultiLineComments(std::string& line, bool stillMultiLineComment);
			--> Functionality to handle multi-line comments when true i.e. adding divs at specific positions in a file

		void handleClassesAndFunctions(std::string fileName, int lineNumber, std::string& line);
			--> Functionality to detect and handle classes and functions i.e. adding divs at specific positions in a file

		int codeConverterParsing(std::vector<std::string> fileList);
			--> Functionality for parsing the given file and walking the globalscope

		std::vector<std::string> uniqueListElements(std::vector<std::string> list1, std::vector<std::string> list2);
			--> To get unique elements from given two lists

		std::vector<lineDataStruct> getUniqueElements(fileDataStruct fileDataStructVariable, std::string);
			--> To get unique elements in a std::unordered_map<std::string, std::vector<lineDataStruct>> data structure

		const std::vector<std::string> convertedFiles() const;
			--> Returns converted files i.e convertedFiles_ variable

		void setGlobalScopeDataStructureValues(CodeAnalysis::ASTNode* pItem);
			--> Sets values for various types in std::unordered_map<int, std::pair<std::string, bool>> data structure

		void clear();
			--> clears convertedFiles_ and dt_ variables

		void callPreHTMLElems(std::string, std::string);
			--> Invoke pre html elements
*
*  Required Files:
* =======================
*  Converter.h Converter.cpp DependencyTable.h DependencyTable.cpp
*  Logger.h Logger.cpp FileSystem.h FileSystem.cpp Utilities.h
*  Utilities.cpp ConfigureParser.h ConfigureParser.cpp
*  DirExplorerN.h DirExplorerN.cpp
*  AbstrSynTree.h AbstrSynTree.cpp Utilities.h Utilities.cpp
*
*  Maintainence History:
* =======================
*  ver 1.0 - 11 Feb 2019
*  - Source
*  - first release
*  ver 1.1 - 14 March 2019
*  - Author
*  - updated release
*/

#include "../DependencyTable/DependencyTable.h"
#include <vector>
#include <string>
#include <fstream>
#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../UtilitiesCodePublisher/UtilitiesCodePublisher.h"

class CodeConverter
{
public:
	using lineDataStruct = std::unordered_map<int, std::pair<std::string, bool>>;
	using fileDataStruct = std::unordered_map<std::string, std::vector<lineDataStruct>>;

	CodeConverter();
	CodeConverter(const DependencyTable& dt);
	const std::string& outputDir() const;
	void outputDir(const std::string& dir);
	bool createOutpurDir();
	void setDepTable(const DependencyTable& dt);
	std::vector<std::string> convert();
	std::string convert(const std::string& filepath);
	std::vector<std::string> convert(const std::vector<std::string>& files);
	void convertListOfFiles(const std::vector<std::string>& files);
	std::string insertSingleToggleButton(int);
	std::string insertDivSingleLineComment(int);
	std::string insertDivMultiLineComment(int);
	std::string insertDivClass(int);
	std::string insertDivFunction(int);
	std::string insertEndingDiv();
	void callPreHTMLElems(std::string, std::string);
	int clearConvertedFilesFolder();
	int setDivId();
	void addAllToggleButtons();
	void createUniqueListAndParse();
	void handleSingleLineComments(std::string& line);
	bool handleMultiLineComments(std::string& line);
	void handleMultiLineComments(std::string& line, bool stillMultiLineComment);
	void handleClassesAndFunctions(std::string fileName, int lineNumber, std::string& line);
	int codeConverterParsing(std::vector<std::string> fileList);
	std::vector<std::string> uniqueListElements(std::vector<std::string> list1, std::vector<std::string> list2);
	std::vector<lineDataStruct> getUniqueElements(fileDataStruct fileDataStructVariable, std::string);
	const std::vector<std::string> convertedFiles() const;
	void setGlobalScopeDataStructureValues(CodeAnalysis::ASTNode* pItem);
	void clear();

private:
	bool convertFile(std::string file);
	void addPreCodeHTML(const std::string& title);
	void addPreTag();
	void addDependencyLinks(std::string file);
	void addClosingTags();
	void skipSpecialChars(std::string& line);
	DependencyTable dt_;
	std::string outputDir_ = "..\\ConvertedWebpages\\";
	std::vector<std::string> convertedFiles_;
	std::ofstream out_;
	int divID = 0;
	bool isMultiLineComment = false;
	std::vector<std::string> filteredFiles_;
	std::vector<std::string> dependentFiles_;
	lineDataStruct lineDivs;
	fileDataStruct fileDivs;
};

