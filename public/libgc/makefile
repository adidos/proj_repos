COMMONDIR=common/
SERVERDIR=server/
PROXYDIR=proxy/
OBJDIR=./lib/
ARLIBDIR=./lib/

CC=g++

INCLUDE:=-I./
SO_CFLAGS:=-Wall -shared -fPIC -O3 $(INCLUDE) $(LOG) -D_DEBUG -std=c++0x
CFLAGS:=-Wall -O0 -g -fPIC $(INCLUDE) $(LOG) -D_DEBUG -std=c++0x

CPPFLAGS:=$(CFLAGS)
LIB=-llog4cplus -lpthread 
# -lssl  : for kernel2.6

SERVERSRC:=$(wildcard $(SERVERDIR)*.cpp)
COMMONSRC:=$(wildcard $(COMMONDIR)*.cpp)
PROXYSRC:=$(wildcard $(PROXYDIR)*.cpp)

SERVEROBJ:=$(patsubst %.cpp,%.o,$(SERVERSRC))
COMMONOBJ:=$(patsubst %.cpp,%.o,$(COMMONSRC))
PROXYOBJ:=$(patsubst %.cpp,%.o,$(PROXYSRC))

SERVERDEP:=$(patsubst %.cpp,%.d,$(SERVERSRC))
COMMONDEP:=$(patsubst %.cpp,%.d,$(COMMONSRC))
PROXYDEP:=$(patsubst %.cpp,%.d,$(COMMONSRC))

CPPFLAGS += -MD

all: $(OBJDIR)libgc.so.1.0 $(ARLIBDIR)libgc.a

deps: $(SERVERDEP) $(COMMONDEP) $(PROXYDEP)

$(OBJDIR)libgc.so.1.0: $(COMMONOBJ) $(SERVEROBJ) $(PROXYOBJ)
	$(CC) $^ $(SO_CFLAGS) -o $@ $(LIB)

$(ARLIBDIR)libgc.a: $(COMMONOBJ) $(SERVEROBJ) $(PROXYOBJ)
	$(AR) rc $@ $^

clean:
	rm $(COMMONOBJ) $(SERVEROBJ) $(PROXYOBJ)  -fv
	rm $(SERVERDEP) $(COMMONDEP)  $(PROXYDEP) -fv
	rm $(OBJDIR)* -fv
	rm $(ARLIBDIR)* -fv

-include $(SERVERDEP) $(COMMONDEP) $(PROXYDEP)

rebuild: clean all
