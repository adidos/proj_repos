TARGET=../bin/hall_svr

MAINDIR=./

CC=g++
INC=-I../../public/libgc/ -I../../public/hiredis/ -I../../public/jsoncpp/include/

LIB=-llog4cplus ../../public/jsoncpp/libs/libjson.a ../../public/hiredis/libhiredis.a -lpthread ../../public/libgc/lib/libgc.a

CFLAGS:=-Wall -O0 -g -lrt -rdynamic $(INC) $(LOG) -D_DEBUG -std=c++0x
CPPFLAGS:=$(CFLAGS)
CPPFLAGS += -MD

MAINCPP:=$(wildcard $(MAINDIR)*.cpp)

MAINOBJ:=$(patsubst %.cpp,%.o,$(MAINCPP))

MAINDEP:=$(patsubst %.cpp,%.d,$(MAINCPP))

all: $(TARGET)

deps: $(MAINDEP)

$(TARGET): $(MAINOBJ)
	$(CC) $^ $(CFLAGS) $(LIB) -o $@ 

main: $(MAINOBJ)
	echo $(MAINOBJ)


clean:
	rm -f $(MAINOBJ)
	rm -f $(MAINDEP) 
	rm -f $(TARGET)
	rm -rf ../log/*

-include $(MAINDEP)

