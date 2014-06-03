
/* ======================================================
* 
* file:		configure.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-02 22:27:27
* 
* ======================================================*/
#ifndef GC_CONFIGURE_H
#define GC_CONFIGURE_H

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>

using namespace std;

class Configure
{
public:
	Configure();
	Configure(const string &file);
	~Configure();

	int loadFile(const string &file);

	string getString(const string &key, const string &value = "");
	int getInt(const string &key, const int &value = 0);
	bool getBool(const string &key, const bool &value = true);
	float getFloat(const string &key, const float &value = 0.0f);

	string dump();

private:
	int parseConfig();

private:
	typedef map<string, string>::iterator Iterator;
	map<string, string> _config_items;
	ifstream _ifs;
};


#endif //GC_CONFIGURE_H
