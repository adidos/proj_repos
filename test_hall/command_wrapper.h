
/* ======================================================
* 
* file:		command_wrapper.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-13 16:57:51
* 
* ======================================================*/
#ifndef COMMAND_WRAPPER_H
#define COMMAND_WRAPPER_H

#include <string>
#include <stdint.h>
using namespace std;

string createGetDir(uint64_t uid, int game_id);

string createGetItem(uint64_t uid, int game_id, int type);

#endif //COMMAND_WRAPPER_H
