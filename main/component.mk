#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

#CXXFLAGS = -std=gnu++14 -DATOMS_NO_EXCEPTION -Wp,-Os,-g3, -mlongcalls

COMPONENT_ADD_INCLUDEDIRS := ./ \
							./sensors \
							./callbacks \
							./sinks \
							./display \
							./IO \
							libs/atoms/include \
							libs/BLE/include \
							libs/BMP280 \
 							libs/Logging/include \
 							libs/MPU/include \
 							libs/SSD1306 \
 							libs/SmartLeds/src \

COMPONENT_SRCDIRS := 		./ \
							./sensors \
							./callbacks \
							./sinks \
							./display \
							./IO \
							libs/atoms/include \
							libs/BLE/include \
							libs/BMP280 \
 							libs/Logging/include \
 							libs/MPU/include \
 							libs/SSD1306 \
 							libs/SmartLeds/src \
