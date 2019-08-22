#pragma once

////////////////////////////////////////////////////////////////////////////////
//  ICodePublisher.h - Provides an interface to interact with when trying to  //
//					   use specific CodePublisher's functionality			  //
//  ver 1.0																	  //
//  Language:      Visual C++, Visual Studio 2017							  //
//  Platform:      Dell XPS 8920, Windows 10								  //
//  Application:   Prototype for CSE687 - OOD Projects						  //
//  Author:		   Vinayak Kukreja, Syracuse University						  //
////////////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package Provides an interface to interact with when trying to use specific CodePublisher's functionality
*
* Public Interface:
* -----------------
*  Class ICodePublisher:

	std::vector<std::string> CodePublisher::GUIPublish(int argc, char** argv)
		--> Function used by the GUI via the interface to use the CodePublisher functionality

	virtual ~ICodePublisher() {}
		--> Destructor

*
*  Required Files:
* =======================
*  ICodePublisher.h ICodePublisher.cpp
*
*  Maintainence History:
* =======================
*  ver 1.0 - 10 April 2019
*  - Author
*/

#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif

#include <vector>

class ICodePublisher
{
public:
	virtual std::vector<std::string> GUIPublish(int argc, char ** argv) = 0;
	virtual ~ICodePublisher() {};
};

extern "C"
{
	struct Objectfactory
	{
		DLL_DECL ICodePublisher* createPublisherInstance();
	};
}