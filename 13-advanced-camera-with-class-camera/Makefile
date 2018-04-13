 
DEFINE	:= 
CFLAGS	:= -Wall -std=gnu++11 -g
LIBS 	:= ../lib
L_LIBS	:= -lstdc++ -lSOIL `pkg-config --libs glfw3 glu` -ldl 
LFLAGS	:= -pipe -pthread

INCLUDES := . /usr/include/libdrm ../include ../models
OBJECTS  := ../commons/glad.o ../commons/application.o
OBJECTS  += $(patsubst %.cpp, %.o, $(wildcard *.cpp))

APP_NAME := $(shell basename $(CURDIR))
RULES := $(wildcard ../rules/*.mk)


all: $(APP_NAME) move_to_bin

include $(RULES)
include $(wildcard *.d) 

 
