TARGET=../bin/hall_svr

MAINDIR=./main/
CMDDIR=./command/

CC=g++
INC=-I/usr/include/mysql -I../../../common/hiredis -I../../../common/libasynframe/common -I../../../common/libasynframe/server -I./command -I./frame -I./common -I../../../common/jsoncpp/include/

LIB=-L../../../common/libasynframe/lib -lasynframe -llog4cplus -lpthread -lcrypto -liconv  ../../../common/hiredis/libhiredis.a ../../../common/jsoncpp/libs/libjson.a
LOG=-DLOGGER -DASYNC_LOGGER

CFLAGS:=-Wall -O0 -g -lrt -rdynamic $(INC) $(LOG) -D_DEBUG 
CPPFLAGS:=$(CFLAGS)
CPPFLAGS += -MD

MAINCPP:=$(wildcard $(MAINDIR)*.cpp)
CMDCPP:=$(wildcard $(CMDDIR)*.cpp)
CMMCPP:=$(wildcard $(CMMDIR)*.cpp)

MAINOBJ:=$(patsubst %.cpp,%.o,$(MAINCPP))
CMDOBJ:=$(patsubst %.cpp,%.o,$(CMDCPP))
FRMOBJ:=$(patsubst %.cpp,%.o,$(FRMCPP))
CMMOBJ:=$(patsubst %.cpp,%.o,$(CMMCPP))

MAINDEP:=$(patsubst %.cpp,%.d,$(MAINCPP))
CMDDEP:=$(patsubst %.cpp,%.d,$(CMDCPP))
CMMDEP:=$(patsubst %.cpp,%.d,$(CMMCPP))

all: $(TARGET)

deps: $(MAINDEP) $(CMDDEP) $(CMMDEP)

$(TARGET): $(MAINOBJ) $(CMDOBJ) $(CMMOBJ)
	$(CC) $^ $(CFLAGS) $(LIB) -o $@ 

main: $(MAINOBJ)
	echo $(MAINOBJ)

common: $(CMMOBJ)

command: $(CMDOBJ)

clean:
	rm $(MAINOBJ) $(CMDOBJ) $(CMMOBJ) -f
	rm $(MAINDEP) $(CMDDEP) $(CMMDEP) -f
	rm $(TARGET) -f

-include $(MAINDEP) $(CMDDEP) $(CMMDEP)

