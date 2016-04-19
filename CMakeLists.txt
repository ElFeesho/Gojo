cmake_minimum_required(VERSION 3.0)

set(DEPS SDL SDL_image SDL_net SDL_mixer SDL_ttf LUA)

set(SOURCES src/main.c src/binds.c src/conv_funcs.c src/errors.c src/binds.h src/config.h src/conv_funcs.h src/errors.h src/globals.h)

add_executable(gojo ${SOURCES})

foreach(DEP ${DEPS})
	find_package(${DEP})
	if ( NOT ${${DEP}_FOUND} )
		message (FATAL_ERROR "${DEP} not found, ${DEP} must be installed to build Gojo")
	else()
		message ("Found ${DEP}")
		string(TOUPPER ${DEP} DEP_UPPER)
		list(APPEND include_dirs ${${DEP}_INCLUDE_DIR} ${${DEP_UPPER}_INCLUDE_DIRS})
		list(APPEND libs ${${DEP}_LIBRARY} ${${DEP_UPPER}_LIBRARIES})
	endif()
endforeach()

if (APPLE)
	list(APPEND libs /usr/local/lib/libSDL_gfx.dylib)
endif()

list(REMOVE_DUPLICATES include_dirs)
list(REMOVE_DUPLICATES libs)

message("Include dirs: ${include_dirs}")
message("libs: ${libs}")
include_directories(${include_dirs})
target_link_libraries(gojo ${libs})

