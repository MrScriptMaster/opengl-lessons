DEFINE	:= 
CFLAGS	:= -Wall -std=gnu++11 -g

INCLUDES := . ../include ../lib/glad/include
OBJECTS  := glad.o
OBJECTS  += $(patsubst %.cpp, %.o, $(wildcard *.cpp))
THIRD_SOURCES := ../lib/glad/src

RULES := $(wildcard ../rules/*.mk)

vpath %.c $(THIRD_SOURCES)

all: $(OBJECTS)

include $(RULES)
include $(wildcard *.d) 