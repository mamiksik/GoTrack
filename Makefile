#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := GoTrackIDF


COMPONENT_ADD_INCLUDEDIRS := components/include

include $(IDF_PATH)/make/project.mk
#-std=gnu++14
CPPFLAGS += -Iatoms/include
CXXFLAGS += -std=c++14 -DATOMS_NO_EXCEPTION -Wp,-Os,-g3
COMPONENT_ADD_LDFLAGS := -Wl,-V