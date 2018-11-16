DEFINE	:= 
CFLAGS	:= -Wall -std=gnu++11 -g

INCLUDES := . ../include
OBJECTS  := glad.o
OBJECTS  += $(patsubst %.cpp, %.o, $(wildcard *.cpp))

RULES := $(wildcard ../rules/*.mk)

all: $(OBJECTS)

include $(RULES)
include $(wildcard *.d) 