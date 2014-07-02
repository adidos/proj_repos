TARGET=test_hall

MAINDIR=./

CC=g++
INC=-I../public/libgc/ 

LIB=-llog4cplus -lpthread ../public/libgc/lib/libgc.a

CFLAGS:=-Wall -O0 -g -std=c++0x $(INC) $(LOG) -D_DEBUG 
CPPFLAGS:=$(CFLAGS)
CPPFLAGS += -MD

MAINCPP:=main.cpp command_wrapper.cpp event_server.cpp

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

-include $(MAINDEP)

