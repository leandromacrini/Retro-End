cmake_minimum_required(VERSION 2.6)

project(retro-end)

#-------------------------------------------------------------------------------
#add local find scripts to CMAKE path
LIST(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Packages
)

#-------------------------------------------------------------------------------
#set up OpenGL system variable
set(GLSystem "Desktop OpenGL" CACHE STRING "The OpenGL system to be used")
set_property(CACHE GLSystem PROPERTY STRINGS "Desktop OpenGL" "OpenGL ES")

#-------------------------------------------------------------------------------
#check if we're running on Raspberry Pi
MESSAGE("Looking for bcm_host.h")
if(EXISTS "/opt/vc/include/bcm_host.h")
    MESSAGE("bcm_host.h found")
    set(BCMHOST found)
    set(GLSystem "OpenGL ES")
else()
    MESSAGE("bcm_host.h not found")
endif()

#finding necessary packages
#-------------------------------------------------------------------------------
if(${GLSystem} MATCHES "Desktop OpenGL")
    find_package(OpenGL REQUIRED)
else()
    find_package(OpenGLES REQUIRED)
endif()
find_package(Freetype REQUIRED)
find_package(FreeImage REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem regex date_time)
find_package(Eigen3 REQUIRED)
find_package(CURL REQUIRED)

#add ALSA for Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(ALSA REQUIRED)
endif()

#-------------------------------------------------------------------------------
#set up compiler flags and excutable names
if(DEFINED BCMHOST)
    add_definitions(-D_RPI_)
endif()

#-------------------------------------------------------------------------------
#set up _WIN32_WINNT variable (used by boost::asio) on Windows
macro(get_WIN32_WINNT version)
    if (WIN32 AND CMAKE_SYSTEM_VERSION)
        set(ver ${CMAKE_SYSTEM_VERSION})
        string(REPLACE "." "" ver ${ver})
        string(REGEX REPLACE "([0-9])" "0\\1" ver ${ver})

        set(${version} "0x${ver}")
    endif()
endmacro()

if(WIN32)
    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})
endif()
#-------------------------------------------------------------------------------

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP") #multi-processor compilation
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP") #multi-processor compilation
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    #check for G++ 4.7+
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE G++_VERSION)
    if (G++_VERSION VERSION_LESS 4.7)
            message(SEND_ERROR "You need at least G++ 4.7 to compile retro-end!")
    endif()
    #set up compiler flags for GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-attributes") #support C++11 for std::, optimize
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")  #strip binary
endif()

if(${GLSystem} MATCHES "Desktop OpenGL")
    add_definitions(-DUSE_OPENGL_DESKTOP)
else()
    add_definitions(-DUSE_OPENGL_ES)
endif()

add_definitions(-DEIGEN_DONT_ALIGN)

#-------------------------------------------------------------------------------
#add include directories
set(ES_INCLUDE_DIRS
    ${FREETYPE_INCLUDE_DIRS}
    ${FreeImage_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
    ${CURL_INCLUDE_DIR}
)

#add ALSA for Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    LIST(APPEND ES_INCLUDE_DIRS
        ${ALSA_INCLUDE_DIRS}
    )
endif()

if(DEFINED BCMHOST)
    LIST(APPEND ES_INCLUDE_DIRS
        "/opt/vc/include"
        "/opt/vc/include/interface/vcos"
        "/opt/vc/include/interface/vmcs_host/linux"
        "/opt/vc/include/interface/vcos/pthreads"
    )
else()
    if(${GLSystem} MATCHES "Desktop OpenGL")
        LIST(APPEND ES_INCLUDE_DIRS
            ${OPENGL_INCLUDE_DIR}
        )
    else()
        LIST(APPEND ES_INCLUDE_DIRS
            ${OPENGLES_INCLUDE_DIR}
        )
    endif()
endif()

#-------------------------------------------------------------------------------
#define basic sources and headers
set(ES_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/src/LIB/pugiXML/pugiconfig.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/LIB/pugiXML/pugixml.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/globals.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer.h

    ${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/AudioController.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/BaseController.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/GamingController.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/InputController.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/LogController.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/RenderController.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/ResourceController.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/ScraperController.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/SettingsController.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/BaseModel.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/Device.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/Font.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/Game.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/ImageIO.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/InputConfig.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/Observer.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/Sound.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/TextureResource.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/BaseView.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/ConsoleView.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/GameInfoView.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/GameListView.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/GamesView.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/Image.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/Label.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/ListView.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/MainWindow.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/MessageDialog.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/MultiChoiceDialog.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/ScrapeView.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/Sprite.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/TestWindow.h

)
set(ES_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/LIB/pugiXML/pugixml.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer_init.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/AudioController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/GamingController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/InputController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/LogController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/RenderController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/RenderControllerSDLGL.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/ResourceController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/ScraperController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Controller/SettingsController.cpp
		
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/BaseModel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/Device.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/Font.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/Game.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/ImageIO.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/InputConfig.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/Sound.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/Model/TextureResource.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/BaseView.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/ConsoleView.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/GameInfoView.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/GameListView.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/GamesView.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/Image.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/Label.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/ListView.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/MainWindow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/MessageDialog.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/MultiChoiceDialog.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/ScrapeView.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/Sprite.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MVC/View/TestWindow.cpp
)

#-------------------------------------------------------------------------------
#define OS specific sources and headers
if(MSVC)
    LIST(APPEND ES_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/src/retro-end.rc
    )
endif()

#-------------------------------------------------------------------------------
#define libraries and directories
if(DEFINED BCMHOST)
    link_directories(
        ${Boost_LIBRARY_DIRS}
        "/opt/vc/lib"
    )
else()
    link_directories(
        ${Boost_LIBRARY_DIRS}
    )
endif()

set(ES_LIBRARIES
    ${Boost_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${FreeImage_LIBRARIES}
	${SDL2_LIBRARY}
    ${SDL2MAIN_LIBRARY}
    ${CURL_LIBRARIES}
)

#add ALSA for Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    LIST(APPEND ES_LIBRARIES
        ${ALSA_LIBRARY}
    )
endif()

if(DEFINED BCMHOST)
    LIST(APPEND ES_LIBRARIES
        bcm_host
        EGL
        ${OPENGLES_LIBRARIES}
    )
else()
    if(MSVC)
        LIST(APPEND ES_LIBRARIES
            winmm
        )
    endif()
    if(${GLSystem} MATCHES "Desktop OpenGL")
        LIST(APPEND ES_LIBRARIES
            ${OPENGL_LIBRARIES}
        )
    else()
        LIST(APPEND ES_LIBRARIES
            ${OPENGLES_LIBRARIES}
        )
    endif()
endif()

#-------------------------------------------------------------------------------
#set up build directories
set(dir ${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)

#-------------------------------------------------------------------------------
#define target
include_directories(${ES_INCLUDE_DIRS})
add_executable(retro-end ${ES_SOURCES} ${ES_HEADERS})
target_link_libraries(retro-end ${ES_LIBRARIES})

#special properties for windows builds
if(MSVC)
    #show console in debug builds, but not in proper release builds
    #Note that up to CMake 2.8.10 this feature is broken: http://public.kitware.com/Bug/view.php?id=12566
    set_target_properties(retro-end PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
    set_target_properties(retro-end PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
    set_target_properties(retro-end PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
    set_target_properties(retro-end PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
    set_target_properties(retro-end PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    set_target_properties(retro-end PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif()