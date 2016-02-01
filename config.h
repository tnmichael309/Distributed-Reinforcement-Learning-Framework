#ifndef CONFIGPARSE_H
#define CONFIGPARSE_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <cstring>

using namespace std;

class ConfigParser {
private:
	string filename;
	map<string, string> settings;
	void parse();
public:
	ConfigParser(string file);
	string get(string name);
	int to_int(string value);
};

#endif