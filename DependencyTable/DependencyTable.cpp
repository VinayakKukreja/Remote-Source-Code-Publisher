///////////////////////////////////////////////////////////////////////////
// DependencyTable.cpp : Defines dependency table structure              //
// ver 1.1                                                               //
//                                                                       // 
// Application   : Student Solution OOD Project-2 Spring 2019            //
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64      //
// Source        : Ammar Salman, Syracuse University                     //
//                 313/788-4694, assalman@syr.edu						 //
// Author		 : Vinayak Kukreja, Syracuse University                  //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines DependencyTable class which is used to hold files
*  relationships after dependency analysis is performed. It is simply a
*  wrapper around std::unordered_map which uses string key and vector
*  value. Therefore, every file in the analysis will point to a vector
*  of all the files it depends upon.
*
* Public Interface:
* -----------------
*  Class DependencyTable:

		DependencyTable();
			--> default ctor

		DependencyTable(const files& list);
			--> files initializer ctor

		~DependencyTable();
			--> dtor - used only for logging

		bool has(const std::string& file);
			--> has function - searches map for given key

		void addDependency(file filename, file dependency);
			--> add dependency function - adds filename to key's vector

		void removeDependency(file filename, file dependency);
			--> remove dependency function

		void removeEntry(file filename);
			--> remove entry - removes key/value pair from map

		const dependencies& getDependencies(file filename) const;
			--> get key's dependencies

		const dependencies& operator[](const file& filename) const;
			--> operator [] to provide easier access

		iterator begin();
			--> begin() iterator - allows for loop

		iterator end();
			--> end() iterator - allows for loop

		void clear() { table_.clear(); }
			--> Clearing contents of the dependency table

		files getFiles() const;
			--> get keys of map

		void display();
			--> display entire table in formatted way

		void coutdisplay();
			--> display entire table in formatted way - std::cout

*
*  Required Files:
* =======================
*  DependencyTable.h DependencyTable.cpp Logger.h Logger.cpp
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

#include "DependencyTable.h"
#include "../LoggerCodePublisher/LoggerCodePublisher.h"
#include <sstream>

using namespace Logging;

// -----< default ctor >---------------------------------------------------
DependencyTable::DependencyTable() {
	LoggerDebug::write("\n  Created instance of DependencyTable using default ctor\n");
}

// -----< files initializer ctor >-----------------------------------------
/* sets the keys for the undordered map */
DependencyTable::DependencyTable(const files & list)
{
	LoggerDebug::write("\n  Created instance of DependencyTable given files list\n");
	table_.reserve(list.size()); // improves performance 
	for (auto file : list)
		table_[file];
}

// -----< dtor - used only for logging >-----------------------------------
DependencyTable::~DependencyTable() {
	LoggerDebug::write("\n  Destroying DependencyTable instance");
}

// -----< has function - searches map for given key >----------------------
bool DependencyTable::has(const std::string & file)
{
	return table_.find(file) != table_.end();
}

// -----< add dependency function - adds filename to key's vector >--------
void DependencyTable::addDependency(file filename, file dependency)
{
	LoggerDebug::write("\n  Adding dependency from " + filename + " to " + dependency);
	// this will create entry if key didnt exists in the map
	DependencyTable::dependencies & deps = table_[filename];
	auto dep = std::find(deps.begin(), deps.end(), dependency);
	if (dep == deps.end()) // don't add depndency if it already exists
	{
		if (dependency == "")
		{
			deps.push_back("");
		}
		else
		{
			deps.push_back(dependency);
		}
	}
}

// -----< remove dependency function >-------------------------------------
void DependencyTable::removeDependency(file filename, file dependency)
{
	LoggerDebug::write("\n  Removing dependency from " + filename + " to " + dependency);
	DependencyTable::dependencies & deps = table_[filename];
	auto dep = std::find(deps.begin(), deps.end(), dependency);
	if (dep != deps.end())
		deps.erase(dep);
}

// -----< remove entry - removes key/value pair from map >-----------------
void DependencyTable::removeEntry(file filename)
{
	LoggerDebug::write("\n  Removing entry [" + filename + "] from dependency table");
	auto entry = table_.find(filename);
	if (entry != table_.end())
		table_.erase(entry);
}

// -----< get key's dependencies >-----------------------------------------
const DependencyTable::dependencies & DependencyTable::getDependencies(file filename) const
{
	return table_.at(filename);
}

// -----< operator [] to provide easier access >---------------------------
const DependencyTable::dependencies & DependencyTable::operator[](const file & filename) const
{
	return table_.at(filename);
}

// -----< begin() iterator - allows for loop >-----------------------------
DependencyTable::iterator DependencyTable::begin()
{
	return table_.begin();
}

// -----< end() iterator - allows for loop >-------------------------------
DependencyTable::iterator DependencyTable::end()
{
	return table_.end();
}

// -----< get keys of map >------------------------------------------------
DependencyTable::files DependencyTable::getFiles() const
{
	files list;
	for (auto entry : table_)
		list.push_back(entry.first);
	return list;
}

// -----< display entire table in formatted way - cout >--------------------------
void DependencyTable::coutdisplay()
{
	std::cout << "\n  Dependency table:";
	for (auto entry : table_) {
		std::cout << "\n  -- " << entry.first;
		for (auto dep : entry.second)
		{
			if (dep == "")
			{
				std::cout << "\n   -> " << "No Dependencies!!";
			}
			else
			{
				std::cout << "\n   -> " << dep;
			}
		}
	}
}

// -----< display entire table in formatted way >--------------------------
void DependencyTable::display()
{
	std::ostringstream out;
	out << "\n  Dependency table:";
	for (auto entry : table_) {
		out << "\n  -- " << entry.first;
		for (auto dep : entry.second)
		{
			if (dep == "")
			{
				out << "\n   -> " << "No Dependencies!!";
			}
			else
			{
				out << "\n   -> " << dep;
			}
		}
	}
	LoggerDebug::write(out.str());
	LoggerDemo::write(out.str());
}

#ifdef TEST_DEBTABLE

#include <iostream>

// ----< test stub for dependency table package >---------------------------
int main(int argc, char ** argv) {
	LoggerDebug::attach(&std::cout);
	LoggerDebug::start();

	LoggerDebug::title("Testing DependencyTable Operations");

	DependencyTable dt;
	dt.addDependency("A.h", "B.h");
	dt.addDependency("A.h", "C.h");
	dt.addDependency("C.h", "B.h");
	dt.addDependency("D.h", "B.h");
	dt.addDependency("E.h", "A.h");
	dt.addDependency("B.h", "E.h");
	dt.addDependency("C.h", "E.h");
	dt.addDependency("E.h", "E.h");
	dt.removeDependency("E.h", "E.h");

	dt.display();

	LoggerDebug::stop();

	return 0;

}

#endif