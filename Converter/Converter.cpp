///////////////////////////////////////////////////////////////////////////
// Converter.cpp   : defines source code conversion to webpage functions //
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

#include <filesystem>
#include "Converter.h"
#include "../LoggerCodePublisher/LoggerCodePublisher.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../UtilitiesCodePublisher/UtilitiesCodePublisher.h"
#include "../CodePublisher/ConfigureParser.h"
#include "../DirExplorer-Naive/DirExplorerN.h"

using namespace FileSystem;
using namespace Logging;
using lineDataStruct = std::unordered_map<int, std::pair<std::string, bool>>;
using fileDataStruct = std::unordered_map<std::string, std::vector<lineDataStruct>>;


// -----< default ctor >--------------------------------------------------
CodeConverter::CodeConverter()
{
}

// -----< ctor to set dep table >-----------------------------------------
CodeConverter::CodeConverter(const DependencyTable & dt) : dt_(dt)
{
	LoggerDebug::write("\n  CodeConverter initialized with DependencyTable\n");
	fileDivs.clear();
	lineDivs.clear();
	createUniqueListAndParse();
}

// -----< set dependency table function >---------------------------------
void CodeConverter::setDepTable(const DependencyTable & dt)
{
	dt_ = dt;
}

// -----< To clear the contents of ConvertedWebpages folder before creating new converted web pages >---------------------------------
int CodeConverter::clearConvertedFilesFolder()
{
	std::string absolutePath = FileSystem::Path::getFullFileSpec(outputDir_);
	std::vector<std::string> directoryList = Directory::getDirectories(absolutePath, "*.*");
	std::vector<std::string> fileList = Directory::getFiles(absolutePath, "*.*");
	try
	{
		if (directoryList.size() != 0)
		{
			for (auto directory : directoryList)
				Directory::remove(directory);
			for (auto file : fileList)
			{
				std::string tempVar = absolutePath  + file;
				File::remove(tempVar);
			}
				
		}
		else
		{
			for (auto file : fileList)
			{
				std::string tempVar = absolutePath + file;
				File::remove(tempVar);
			}
		}
		return 1;
	}
	catch (...) { return 0; }
}

// -----< Increases the counter to add as unique id for requiring HTML elements >---------------------------------
int CodeConverter::setDivId()
{
	return CodeConverter::divID++;
}


// -----< To get unique elements from given two lists >---------------------------------

std::vector<std::string> CodeConverter::uniqueListElements(std::vector<std::string> list1, std::vector<std::string> list2)
{
	std::vector<std::string> uniqueList;
	std::sort(list1.begin(), list1.end());
	std::sort(list2.begin(), list2.end());
	std::set_symmetric_difference(list1.begin(), list1.end(), list2.begin(), list2.end(), std::back_inserter(uniqueList));
	sort(uniqueList.begin(), uniqueList.end());
	uniqueList.erase(unique(uniqueList.begin(), uniqueList.end()), uniqueList.end());
	return uniqueList;

}

// -----< Creates a new list with unique elements with the help of uniqueListElements() >---------------------------------
void CodeConverter::createUniqueListAndParse()
{
	for (auto entry : dt_)
	{
		filteredFiles_.push_back(entry.first);
		std::vector<std::string> keyValues = dt_[entry.first];
		for (auto file : keyValues)
			dependentFiles_.push_back(file);
		std::vector<std::string> uniqueElements = uniqueListElements(filteredFiles_, dependentFiles_);
		int x = codeConverterParsing(uniqueElements);
	}
}


// -----< convert pre-set dependency table >------------------------------
std::vector<std::string> CodeConverter::convert()
{
	if (!createOutpurDir()) {
		LoggerDebug::write("\n\n  Failed to start conversion due to inability to create output directory");
		LoggerDebug::write("\n  -- Output Directory: " + Path::getFullFileSpec(outputDir_));
		return std::vector<std::string>();
	}
	LoggerDemo::write("\n\n  Converting files to webpages\n");
	LoggerDebug::write("\n\n  Converting files to webpages\n");
	LoggerDebug::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));
	LoggerDemo::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));
	for (auto entry : dt_) {
		if (convertFile(entry.first)) {
			filteredFiles_.push_back(entry.first);
			std::string toDisplayFiles = Path::getFullFileSpec(outputDir_) + Path::getName(entry.first) + ".html";
			convertedFiles_.push_back(Path::getFullFileSpec(toDisplayFiles));
			LoggerDemo::write("\n  -- Converted: ");
			LoggerDebug::write("\n  -- Converted: ");
			std::vector<std::string> keyValues = dt_[entry.first];
			if (keyValues.size() == 1 && keyValues[0] == "")
				continue;
			for (auto file : keyValues)
				dependentFiles_.push_back(file);
			convertListOfFiles(keyValues);
		}
		else {
			LoggerDemo::write("\n  -- Failed:    ");
			LoggerDebug::write("\n  -- Failed:    ");
		}
		std::string filename = Path::getName(entry.first);

		LoggerDebug::write(filename);
		LoggerDemo::write(filename);
	}
	LoggerDebug::write("\n");
	LoggerDemo::write("\n");
	return convertedFiles_;
}

// -----< convert single file given path >----------------------------------
std::string CodeConverter::convert(const std::string & filepath)
{
	if (!createOutpurDir()) 
	{
		LoggerDebug::write("\n\n  Failed to start conversion due to inability to create output directory");
		LoggerDebug::write("\n  -- Output Directory: " + Path::getFullFileSpec(outputDir_));
		return "";
	}
	LoggerDemo::write("\n\n  Converting files to webpages");
	LoggerDebug::write("\n\n  Converting files to webpages");
	LoggerDebug::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));
	LoggerDemo::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));
	if (convertFile(filepath)) 
	{
		LoggerDemo::write("\n  -- Converted: ");
		LoggerDebug::write("\n  -- Converted: ");
	}
	else 
	{
		LoggerDemo::write("\n  -- Failed:    ");
		LoggerDebug::write("\n  -- Failed:    ");
	}
	std::string filename = Path::getName(filepath);
	LoggerDebug::write(filename);
	LoggerDemo::write(filename);
	return filename + ".html";
}

// -----< Sets dependency table to files and converts >---------------------------------
std::vector<std::string> CodeConverter::convert(const std::vector<std::string>& files)
{
	clear();
	dt_ = DependencyTable(files);
	return convert();
}

// -----< get set output directory >----------------------------------------
const std::string & CodeConverter::outputDir() const
{
	return outputDir_;
}

// -----< set output directory >--------------------------------------------
void CodeConverter::outputDir(const std::string & dir)
{
	outputDir_ = dir;
}

// -----< create output directory >-----------------------------------------
/* returns success of creation. it could fail if dir is sys protected */
bool CodeConverter::createOutpurDir()
{
	if (!Directory::exists(outputDir_)) 
	{
		LoggerDebug::write("\n  Creating output directory. Path: " + Path::getFullFileSpec(outputDir_));
		return Directory::create(outputDir_);
	}
	LoggerDebug::write("\n  Output directory already exists. Proceeding...");
	return true;
}

// -----< get list of converted files >-------------------------------------
const std::vector<std::string> CodeConverter::convertedFiles() const
{
	return convertedFiles_;
}

// -----< Clearing converted files >---------------------------------
void CodeConverter::clear()
{
	convertedFiles_.clear();
	dt_.clear();
}

// -----< Converts a list of files provided >---------------------------------
void CodeConverter::convertListOfFiles(const std::vector<std::string>& files)
{
	for (auto file : files)
	{
		if (!convertFile(file))
		{
			std::cout << "Error in converting file" << std::endl;
		}
	}
}

// -----< Invoke pre HTML elements >---------------------------------
void CodeConverter::callPreHTMLElems(std::string filename, std::string file)
{
	addPreCodeHTML(filename);
	addAllToggleButtons();
	addDependencyLinks(file);
	addPreTag();
}


// -----< private - read file and create webpage >--------------------------
bool CodeConverter::convertFile(std::string file)
{
	std::ifstream in(file);
	isMultiLineComment = false;
	if (!in.is_open() || !in.good()) 
	{
		LoggerDebug::write("Error -- unable to read file, path may be invalid.");
		in.close();
		return false;
	}
	std::string filename = Path::getName(file);
	std::string outputPath = outputDir_ + filename + ".html";
	out_.open(outputPath, std::ofstream::out);
	if (!out_.is_open() || !out_.good()) 
	{
		LoggerDebug::write("Error -- unable to open output file for writing.");
		in.close();
		return false;
	}
	callPreHTMLElems(filename, file);
	size_t count = 0;
	while (in.good()) {
		std::string line;
		while (std::getline(in, line)) 
		{
			count++;
			skipSpecialChars(line);
			if (!isMultiLineComment)
			{
				isMultiLineComment = handleMultiLineComments(line);
				if (isMultiLineComment)
				{
					out_ << line << std::endl;
					continue;
				}
				handleSingleLineComments(line);
				handleClassesAndFunctions(filename, count, line);
			}
			else
			{

				handleMultiLineComments(line, isMultiLineComment);
			}
			out_ << line << std::endl;
		}
	}
	addClosingTags();
	out_.close();
	return true;
}

// -----< Inserts a HTML button at a given position with specific id >---------------------------------
std::string CodeConverter::insertSingleToggleButton(int id)
{
	return "<button class=\"btn danger singleButton\" onclick='trigger(\"div-" + std::to_string(id) + "\", this);' value=\"-\">-</button>";
}

// -----< Inserts a HTML div with Comment as the class at a given position with specific id >---------------------------------
std::string CodeConverter::insertDivSingleLineComment(int id)
{
	return "<div style=\"display:inline;\" id=\"div-" + std::to_string(id) + "\" class=\"Comments\">";
}

// -----< Inserts a HTML div with Comment as the class at a given position with specific id >---------------------------------
std::string CodeConverter::insertDivMultiLineComment(int id)
{
	return "<div style=\"display:inline;\" id=\"div-" + std::to_string(id) + "\" class=\"Comments\">";
}

// -----< Inserts a HTML div with Classes as the class at a given position with specific id >---------------------------------
std::string CodeConverter::insertDivClass(int id)
{
	return "<div style=\"display:inline;\" id=\"div-" + std::to_string(id) + "\" class=\"Classes\">";
}

// -----< Inserts a HTML div with Functions as the class at a given position with specific id >---------------------------------
std::string CodeConverter::insertDivFunction(int id)
{
	return "<div style=\"display:inline;\" id=\"div-" + std::to_string(id) + "\" class=\"Functions\">";
}

// -----< Inserts a HTML ending div i.e. </div> >---------------------------------
std::string CodeConverter::insertEndingDiv()
{
	return "</div>";
}


// -----< private - add generic HTML preliminary markup >-------------------
void CodeConverter::addPreCodeHTML(const std::string& title)
{
	out_ << "<DOCTYPE !HTML>" << std::endl;
	out_ << "<html>" << std::endl;
	out_ << "  <head>" << std::endl;
	out_ << "    <Title>" << title << "</Title>" << std::endl;
	out_ << "	   <meta charset=\"UTF-8\"/>" << std::endl;
	out_ << "    <style>" << std::endl;
	out_ << "      body {" << std::endl;
	out_ << "        padding:15px 40px;" << std::endl;
	out_ << "        font-family: Consolas;" << std::endl;
	out_ << "        font-size: 1.25em;" << std::endl;
	out_ << "        font-weight: normal;" << std::endl;
	out_ << "      }" << std::endl;
	out_ << "      </style>" << std::endl;
	out_ << "		 <script src=\"../script.js\"></script>" << std::endl;
	out_ << "		 <link rel=\"stylesheet\" type=\"text/css\" href=\"../Bootstrap-4/css/bootstrap.min.css\" />" << std::endl;
	out_ << "		 <link rel=\"stylesheet\" type=\"text/css\" href=\"../Bootstrap-4/css/bootstrap-theme.min.css\" />" << std::endl;
	out_ << "		 <link href=\"../style.css\" rel=\"stylesheet\" />" << std::endl;
	out_ << "  </head>" << std::endl << std::endl;
	out_ << "  <body>" << std::endl;
}

// -----< To add buttons that can toggle all specific elements such as functions, comments, classes, at a  specific position >---------------------------------
void CodeConverter::addAllToggleButtons()
{
	out_ << "      <div class=\"absolute\">";
	out_ << "           <button class=\"btn btn-danger all-button\" onclick=\"toggleVisibility('Comments');\">Comments</button>";
	out_ << "           <button class=\"btn btn-danger all-button\" onclick=\"toggleVisibility('Functions');\">Functions</button>";
	out_ << "           <button class=\"btn btn-danger all-button\" onclick=\"toggleVisibility('Classes');\">Classes</button>";
	out_ << "		</div>";
}

// -----< private - add pre tag >------------------------------------------
/* seperated into seperate function to allow for dependencies addition
*  before the actual code of the file */
void CodeConverter::addPreTag()
{
	out_ << "    <pre>" << std::endl;
}

// -----< private - add depedency links markup code >----------------------
void CodeConverter::addDependencyLinks(std::string file)
{
	std::string filename = Path::getName(file);
	if (!dt_.has(file)) 
	{ // in case of single file conversion
		LoggerDebug::write("\n    No entry found in DependencyTable for [" + filename + "]. Skipping dependency links..");
		return;
	}
	if (dt_[file].size() == 0) 
	{ // in case the file has no dependencies
		LoggerDebug::write("\n    No dependencies found for [" + filename + "]. Skipping dependency links..");
		return;
	}
	out_ << "<br /><div>    <h1><u>Dependencies:</u> <br /><br />" << std::endl;
	for (auto dep : dt_[file]) 
	{
		if (dep == "")
		{
			out_ << "      <a href=\"\"><h3><i>" << "No Dependencies!!" << "</i></h3></a>    " << std::endl;
		}
		else
		{
			out_ << "      <a href=\"" << Path::getName(dep) << ".html\"><h3><i>" << Path::getName(dep) << "</i></h3></a>    " << std::endl;
		}
	}
	out_ << "    </h1></div><br /><br />";
}

// -----< private - add generic HTML markup closing tags >-----------------
void CodeConverter::addClosingTags()
{
	out_ << "    </pre>" << std::endl;
	out_ << "  </body>" << std::endl;
	out_ << "</html>" << std::endl;
}

// -----< private - replace HTML special chars >---------------------------
/* note: the function uses while loop to make sure ALL special characters
*  are replaced instead of just the first encounter. */
void CodeConverter::skipSpecialChars(std::string & line)
{
	size_t pos = line.npos;
	while ((pos = line.find('<')) != line.npos)
		line.replace(pos, 1, "&lt;");

	while ((pos = line.find('>')) != line.npos)
		line.replace(pos, 1, "&gt;");
}

// -----< Functionality to detect and handle comments i.e. adding divs at specific positions in a file >---------------------------------
void CodeConverter::handleSingleLineComments(std::string& line)
{
	size_t pos = line.find('/');
	if (pos == line.npos)
		return;
	if (pos == line.size() - 1)
		return;
	if (line[pos + 1] == '/')
	{
		int id = setDivId();
		line.insert(pos, insertSingleToggleButton(id) + insertDivSingleLineComment(id));
		line += insertEndingDiv();
	}
}

// -----< Functionality to detect and handle multi-line comments i.e. adding divs at specific positions in a file >---------------------------------
bool CodeConverter::handleMultiLineComments(std::string& line)
{
	size_t pos = line.find('/');
	if (pos == line.npos)
		return false;
	if (pos == line.size() - 1)
		return false;
	if (line[pos + 1] == '*')
	{
		int id = setDivId();
		line.insert(pos, insertSingleToggleButton(id) + insertDivMultiLineComment(id));
		size_t pos_last = line.find_last_of('*');
		if (line[pos_last + 1] == '/' && pos != pos_last)
		{
			line.insert(pos_last + 2, insertEndingDiv());
			return false;
		}
		return true;
	}
	return false;
}

// -----< Functionality to handle multi-line comments when true i.e. adding divs at specific positions in a file >---------------------------------
void CodeConverter::handleMultiLineComments(std::string& line, bool stillMultiLineComment)
{
	if (!stillMultiLineComment)
	{

	}
	else
	{
		size_t pos = line.find('*');
		if (line[pos + 1] == '/')
		{
			line += insertEndingDiv();
			isMultiLineComment = false;
		}
	}
}

// -----< To get unique elements in a std::unordered_map<std::string, std::vector<lineDataStruct>> data structure >---------------------------------
std::vector<lineDataStruct> CodeConverter::getUniqueElements(fileDataStruct fileData, std::string filename)
{
	std::vector<lineDataStruct> pairList = fileData[filename];
	std::vector<lineDataStruct> uniquePairList;
	std::vector<lineDataStruct> duplicatePairList;
	for (auto pair1 : pairList)
	{
		int count = 0;
		int flag = 0;

		for (auto duplicates : duplicatePairList)
		{
			if (pair1 == duplicates)
			{
				flag = 1;
			}
		}
		if (flag == 1)
			break;
		for (auto pair2 : pairList)
		{
			if (pair1 == pair2)
			{
				count += 1;
				if (count == 1)
				{
					uniquePairList.push_back(pair1);
				}
				if (count > 1)
				{
					duplicatePairList.push_back(pair1);
					break;
				}
			}
		}
	}
	return uniquePairList;
}

// -----< Functionality to detect and handle classes and functions i.e. adding divs at specific positions in a file >---------------------------------
void CodeConverter::handleClassesAndFunctions(std::string fileName, int lineNumber, std::string& line)
{
	std::vector<lineDataStruct> pairList = getUniqueElements(fileDivs, fileName);

	for (auto pairs : pairList)
	{
		if (pairs[lineNumber].first == "class")
		{
			if (pairs[lineNumber].second == true)
			{
				int id = setDivId();
				line.insert(0, insertSingleToggleButton(id) + insertDivClass(id));
			}
			if (pairs[lineNumber].second == false)
			{
				size_t pos = line.find_last_of('}');
				if (line[pos + 1] == ';')
				{
					line.insert(pos + 2, insertEndingDiv());
				}
			}
		}
		if (pairs[lineNumber].first == "function")
		{
			if (pairs[lineNumber].second == true)
			{
				int id = setDivId();
				size_t posStart = line.find_first_of('{');
				size_t posEnd = line.find_last_of('}');
				line.insert(0, insertSingleToggleButton(id) + insertDivFunction(id));
			}
			if (pairs[lineNumber].second == false)
			{
				size_t pos = line.find('}');
				size_t posStart = line.find('{');
				size_t posEnd = line.find('}');

				if (posEnd != -1 && posStart != -1)
				{
					int id = setDivId();
					line.insert(0, insertSingleToggleButton(id) + insertDivFunction(id));
					pos = line.find('}');
					line.insert(pos + 1, insertEndingDiv());
				}
				else
					line.insert(pos + 1, insertEndingDiv());
			}
		}
	}
}

// -----< Sets values for various types in std::unordered_map<int, std::pair<std::string, bool>> data structure >---------------------------------
void CodeConverter::setGlobalScopeDataStructureValues(CodeAnalysis::ASTNode* pItem)
{
	static std::string path = pItem->path_;
	static size_t indentLevel = 0;
	auto iter = pItem->children_.begin();
	while (iter != pItem->children_.end())
	{
		auto temp = *iter;
		std::string type = temp->type_;
		if (type == "class")
		{
			int flag = 0;
			int startPosition = temp->startLineCount_;
			int endPosition = temp->endLineCount_;
			lineDivs[startPosition] = std::pair<std::string, bool>(type, true);
			lineDivs[endPosition - 1] = std::pair<std::string, bool>(type, false);
			std::string name = temp->package_;
			fileDivs[name].push_back(lineDivs);
			lineDivs.clear();
		}
		else if (type == "function")
		{
			int flag = 0;
			int startPosition = temp->startLineCount_;
			int endPosition = temp->endLineCount_;
			lineDivs[startPosition] = std::pair<std::string, bool>(type, true);
			lineDivs[endPosition] = std::pair<std::string, bool>(type, false);
			std::string name = temp->package_;
			fileDivs[name].push_back(lineDivs);
			lineDivs.clear();
		}
		setGlobalScopeDataStructureValues(*iter);
		++iter;
	}
}

// -----< Functionality for parsing the given file and walking the globalscope >---------------------------------
int CodeConverter::codeConverterParsing(std::vector<std::string> fileList)
{
	std::string fileSpec;
	std::vector<std::string> fileListTemp = fileList;

	for (auto file : fileListTemp)
	{
		if (file == "")
		{
			continue;
		}
		fileSpec = FileSystem::Path::getFullFileSpec(file);
		CodeAnalysis::ConfigParseForCodeAnal configure;
		CodeAnalysis::Parser* pParser = configure.Build();
		std::string name;
		try
		{
			if (pParser)
			{
				name = FileSystem::Path::getName(file);
				if (!configure.Attach(fileSpec))
				{
					std::cout << "\n  could not open file " << name << std::endl;
					continue;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
				return 1;
			}
			CodeAnalysis::Repository* pRepo = CodeAnalysis::Repository::getInstance();
			pRepo->package() = name;
			while (pParser->next())
			{
				pParser->parse();
			}
			CodeAnalysis::ASTNode* pGlobalScope = pRepo->getGlobalScope();
			setGlobalScopeDataStructureValues(pGlobalScope);
		}
		catch (std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
			std::cout << "\n  exception caught at line " << __LINE__ << " ";
			std::cout << "\n  in package \"" << name << "\"";
			return 0;
		}
	}
	return 1;
}


#ifdef TEST_CONVERTER

// -----< test stub for converter class >-----------------------------------
int main() {
	LoggerDebug::attach(&std::cout);
	LoggerDebug::start();
	LoggerDebug::title("Testing CodeConverter functions");
	LoggerDebug::stop();
	DependencyTable dt;
	std::string currentDirectory = Directory::getCurrentDirectory();
	std::string file1 = Path::getFullFileSpec("../Logger/Logger.h");
	std::string file2 = Path::getFullFileSpec("../Logger/Logger.cpp");
	dt.addDependency(file1, file2);
	dt.addDependency(file2, file1);
	LoggerDebug::start(); // log from now on
	CodeConverter cc(dt);
	dt.display();
	LoggerDebug::write("\n");
	cc.convert();
	LoggerDebug::stop();
	return 0;
}

#endif