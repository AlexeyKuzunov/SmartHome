#############################################################################
#
# Makefile for RF24SN C++ Server
#
# License: MIT
# Author:  Vaclav Synacek
# Date:    2014 (version 1.0)
#
# Description:
# ------------
# use make all and make install to install
# You can change the install directory by editing the prefix line
# Requires librf24 installed
#


# The recommended compiler flags for the Raspberry Pi
#CC_FLAGS=-Wall -ggdb -I./RF24/ -Ofast -mfpu=vfp -mfloat-abi=hard
CC_FLAGS=-Wall -ggdb -I./RF24/ -mfpu=vfp -mfloat-abi=hard
LD_FLAGS=-Wall -ggdb -lrf24-sun7i -lmosquitto
CPP_FILES=$(wildcard *.cpp)
OBJ_FILES=$(CPP_FILES:.cpp=.o)

%.o: %.cpp
	g++ $(CC_FLAGS) -c -o $@ $<
   
all: $(OBJ_FILES)
	g++ -o RF24SN $^ $(LD_FLAGS)

clean:
	rm -rf RF24SN *.o *.d *.log


CC_FLAGS += -MMD
-include $(OBJFILES:.o=.d)