#pragma once

// These numbers and strings need to be manually updated for a new version.
// Do this version number update as the very last commit for the new release version.
#define PROGRAM_VERSION_MAJOR		1
#define PROGRAM_VERSION_MINOR		0
#define PROGRAM_VERSION_MAINTENANCE	0
#define PROGRAM_VERSION_REVISION	0
#define PROGRAM_VERSION_STRING PROGRAM_VERSION_MAJOR+"."+PROGRAM_VERSION_MINOR+"."+PROGRAM_VERSION_MAINTENANCE+"."+PROGRAM_VERSION_REVISION
#define PROGRAM_VERSION_STRING_BUILTDATE PROGRAM_VERSION_MAJOR+"."+PROGRAM_VERSION_MINOR+"."+PROGRAM_VERSION_MAINTENANCE+"."+PROGRAM_VERSION_REVISION+" - built " __DATE__ " - " __TIME__
#define RESOURCE_VERSION_STRING "1,0,0,0\0"

#define RESOURCE_VERSION PROGRAM_VERSION_MAJOR,PROGRAM_VERSION_MINOR,PROGRAM_VERSION_MAINTENANCE,PROGRAM_VERSION_REVISION

//global includes
#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

//the Makefiles define these via command line
//#define USE_OPENGL_ES
//#define USE_OPENGL_DESKTOP

#ifdef USE_OPENGL_ES
	#define GLHEADER <GLES/gl.h>
#endif

#ifdef USE_OPENGL_DESKTOP
	//why the hell this naming inconsistency exists is well beyond me
	#ifdef WIN32
		#define sleep Sleep
	#endif

	#define GLHEADER <SDL_opengl.h>
#endif

using namespace std;

namespace RetroEnd
{
	const string DB_NAME = "data/retro.db";

	const string APP_DIRECTORY = "retro-end";
}

//a function used to replace part of a string
static inline string strreplace(string& str, string replace, string with)
{
	size_t start_pos = 0;
	string result =str;

	while((start_pos = result.find(replace, start_pos)) != std::string::npos) {
         result = result.replace(start_pos, replace.length(), with);
         start_pos += with.length();
	}

	return result;
}

// trim from start
static inline string& ltrim(string &s) {
        s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
        return s;
}

// trim from end
static inline string& rtrim(string &s) {
        s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline string& trim(string &s) {
        return ltrim(rtrim(s));
}