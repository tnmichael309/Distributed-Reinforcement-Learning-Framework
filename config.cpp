#include "config.h"

void ConfigParser::parse() {
	ifstream config_file;
	config_file.open(filename.c_str());
	string temp;
	while (getline(config_file, temp)) {
		int cut_pos = temp.find("=");
		string setting_name = temp.substr(0, cut_pos);
		string setting_value = temp.substr(cut_pos + 1);
		settings[setting_name] = setting_value;
	}
	config_file.close();
}

ConfigParser::ConfigParser(string file) {
	settings.clear();
	filename = file;
	parse();
}

string ConfigParser::get(string name) {
	return settings[name];
}

int ConfigParser::to_int(string value) {
	stringstream ss(value);
	int result;
	ss >> result;
	return result;
}