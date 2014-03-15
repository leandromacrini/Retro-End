# - find Websocketpp
# WEBSOCKETPP_LIBRARY_RELEASE - Where the release library is
# WEBSOCKETPP_LIBRARY_DEBUG - Where the debug library is
# WEBSOCKETPP_FOUND - Set to TRUE if we found everything


FIND_LIBRARY(WEBSOCKETPP_LIBRARY_RELEASE NAMES websocketpp )

FIND_LIBRARY(WEBSOCKETPP_LIBRARY_DEBUG NAMES websocketpp)

IF( WEBSOCKETPP_LIBRARY_RELEASE AND WEBSOCKETPP_LIBRARY_DEBUG )
    SET(WEBSOCKETPP_FOUND TRUE)
	IF( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
		SET( WEBSOCKETPP_LIBRARIES optimized ${WEBSOCKETPP_LIBRARY_RELEASE} debug ${WEBSOCKETPP_LIBRARY_DEBUG} )
	ELSE( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
    # if there are no configuration types and CMAKE_BUILD_TYPE has no value
    # then just use the release libraries
		SET( WEBSOCKETPP_LIBRARIES ${WEBSOCKETPP_LIBRARY_RELEASE} )
	ENDIF( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
ENDIF( WEBSOCKETPP_LIBRARY_RELEASE AND WEBSOCKETPP_LIBRARY_DEBUG )

IF( SQLITE3_LIBRARY_DEBUG AND SQLITE3_LIBRARY_RELEASE )
	# if the generator supports configuration types then set
	# optimized and debug libraries, or if the CMAKE_BUILD_TYPE has a value
	IF( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
		SET( SQLITE3_LIBRARIES optimized ${SQLITE3_LIBRARY_RELEASE} debug ${SQLITE3_LIBRARY_DEBUG} )
	ELSE( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
    # if there are no configuration types and CMAKE_BUILD_TYPE has no value
    # then just use the release libraries
		SET( SQLITE3_LIBRARIES ${SQLITE3_LIBRARY_RELEASE} )
	ENDIF( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
ELSEIF( SQLITE3_LIBRARY_RELEASE )
	SET( SQLITE3_LIBRARIES ${SQLITE3_LIBRARY_RELEASE} )
ELSE( SQLITE3_LIBRARY_DEBUG AND SQLITE3_LIBRARY_RELEASE )
	SET( SQLITE3_LIBRARIES ${SQLITE3_LIBRARY_DEBUG} )
ENDIF( SQLITE3_LIBRARY_DEBUG AND SQLITE3_LIBRARY_RELEASE )

IF( WEBSOCKETPP_FOUND )
	MESSAGE( STATUS "Found Wepsocketpp libraries")
ELSE(WEBSOCKETPP_FOUND)
	MESSAGE( FATAL_ERROR "Could not find Websocket" )
ENDIF(WEBSOCKETPP_FOUND)