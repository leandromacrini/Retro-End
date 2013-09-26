#pragma once

// These numbers and strings need to be manually updated for a new version.
// Do this version number update as the very last commit for the new release version.
#define PROGRAM_VERSION_MAJOR       1
#define PROGRAM_VERSION_MINOR        0
#define PROGRAM_VERSION_MAINTENANCE  0
#define PROGRAM_VERSION_REVISION     0
#define PROGRAM_VERSION_STRING PROGRAM_VERSION_MAJOR+"."+PROGRAM_VERSION_MINOR+"."+PROGRAM_VERSION_MAINTENANCE+"."+PROGRAM_VERSION_REVISION
#define PROGRAM_VERSION_STRING_BUILTDATE PROGRAM_VERSION_MAJOR+"."+PROGRAM_VERSION_MINOR+"."+PROGRAM_VERSION_MAINTENANCE+"."+PROGRAM_VERSION_REVISION+" - built " __DATE__ " - " __TIME__
#define RESOURCE_VERSION_STRING "1,0,0,0\0"

#define RESOURCE_VERSION PROGRAM_VERSION_MAJOR,PROGRAM_VERSION_MINOR,PROGRAM_VERSION_MAINTENANCE,PROGRAM_VERSION_REVISION

//global includes
#include <vector>
#include <iostream>
#include <string>

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
	const string DB_NAME = "retro.db";

	const string APP_DIRECTORY = "retro-end";
}
