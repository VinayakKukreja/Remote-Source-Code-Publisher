#pragma once

///////////////////////////////////////////////////////////////////////////
// DependencyTable.h : Defines dependency table structure                //
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

#include <unordered_map>
#include <string>
#include <vector>

class DependencyTable
{
public:
	using file = std::string;
	using files = std::vector<file>;
	using dependencies = std::vector<file>;
	using iterator = std::unordered_map<file, dependencies>::iterator;

	DependencyTable();
	DependencyTable(const files& list);
	~DependencyTable();
	bool has(const std::string& file);
	void addDependency(file filename, file dependency);
	void removeDependency(file filename, file dependency);
	void removeEntry(file filename);
	const dependencies& getDependencies(file filename) const;
	const dependencies& operator[](const file& filename) const;
	iterator begin();
	iterator end();
	void clear() { table_.clear(); }
	files getFiles() const;
	void display();
	void coutdisplay();

private:
	std::unordered_map<file, dependencies> table_;
};

