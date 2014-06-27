
/* ======================================================
* 
* file:		utility.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-02 23:54:05
* 
* ======================================================*/
#ifndef GC_COMMON_H
#define GC_COMMON_H

#include <string.h>
#include <stdint.h>

#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <vector>

using namespace std;


string trim_left_blank(const string &str);
string trim_right_blank(const string &str);
string trim_string(const string & str);
int split_string(const string& source, const char* delimitor, vector<string>& result_array);

bool isDigit(const string& str);
bool isDecimal(const string &str);

int64_t current_time_sec();
int64_t current_time_ms();
int64_t current_time_usec();

string current_date();
string current_datetime();

int setNoBlock(int fd);


#endif //GC_COMMON_H
