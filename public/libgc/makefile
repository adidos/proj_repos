COMMONDIR=common/
SERVERDIR=server/
OBJDIR=./build/
ARLIBDIR=./lib/

CC=g++

INCLUDE:=-I./
SO_CFLAGS:=-Wall -shared -fPIC -O3 $(INCLUDE) $(LOG) -D_DEBUG
CFLAGS:=-Wall -O2 -g -fPIC $(INCLUDE) $(LOG) -D_DEBUG -D_FILE_OFFSET_BITS=64

CPPFLAGS:=$(CFLAGS)
LIB=-llog4cplus -lpthread 
# -lssl  : for kernel2.6

SERVERSRC:=$(wildcard $(SERVERDIR)*.cpp)
COMMONSRC:=$(wildcard $(COMMONDIR)*.cpp)

SERVEROBJ:=$(patsubst %.cpp,%.o,$(SERVERSRC))
COMMONOBJ:=$(patsubst %.cpp,%.o,$(COMMONSRC))

SERVERDEP:=$(patsubst %.cpp,%.d,$(SERVERSRC))
COMMONDEP:=$(patsubst %.cpp,%.d,$(COMMONSRC))

CPPFLAGS += -MD

all: $(OBJDIR)libgc.so.1.0 $(ARLIBDIR)libgc.a

deps: $(SERVERDEP) $(COMMONDEP) 

$(OBJDIR)libgc.so.1.0: $(COMMONOBJ) $(SERVEROBJ) 
	$(CC) $^ $(SO_CFLAGS) -o $@ $(LIB)

$(ARLIBDIR)libgc.a: $(COMMONOBJ) $(SERVEROBJ)
	$(AR) rc $@ $^

clean:
	rm $(COMMONOBJ) $(SERVEROBJ)  -fv
	rm $(SERVERDEP) $(COMMONDEP)  -fv
	rm $(OBJDIR)* -fv
	rm $(ARLIBDIR)* -fv

-include $(SERVERDEP) $(COMMONDEP) 

rebuild: clean all
