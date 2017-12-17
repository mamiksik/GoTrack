cmake_minimum_required(VERSION 3.2)
project(GoTrack)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(CMakeListsPrivate.txt)

add_custom_target(
		BUILD ALL
		COMMAND make -j4
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
		BUILD_AND_FLASH ALL
		COMMAND make flash -j4
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
		SETTINGS ALL
		COMMAND make menuconfig
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
		REINDEX_LIBS ALL
		COMMAND ./rebuild_indexes.py
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)


add_executable(GoTrack ${ARDUINO_LIST} ${SRC_LIST})