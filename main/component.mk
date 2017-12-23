#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

#CXXFLAGS = -std=gnu++14 -DATOMS_NO_EXCEPTION -Wp,-Os,-g3, -mlongcalls

COMPONENT_ADD_INCLUDEDIRS := ./ \
							libs/atoms/include \
							libs/BLE/include \
							libs/BMP280 \
 							libs/Logging/include \
 							libs/MPU/include \
 							libs/SSD1306 \

COMPONENT_SRCDIRS := 		./ \
							libs/atoms/include \
							libs/BLE/include \
							libs/BMP280 \
 							libs/Logging/include \
 							libs/MPU/include \
 							libs/SSD1306 \
