#include "transferStatus.h"
#include "config.h"

string SERVER_IP; // = "140.113.86.124";
int SERVER_PORT; // 	= 1000;
int SYNC_OLD_COUNT ; // = 5;
string WEIGHT_FILE_NAME; //  = "testingWeights.txt";
string OLD_WEIGHT_FILE_NAME; // = "testingWeights_old.txt";


void init_parameters() {

	ConfigParser cp("DST_DRL.ini");
	
	string test = cp.get("SERVER_IP");
	//cerr << test.size() << endl;
	//cerr << test[test.size()-1] << endl;
	
	SERVER_IP = cp.get("SERVER_IP");
	WEIGHT_FILE_NAME = cp.get("WEIGHT_FILE_NAME");
	OLD_WEIGHT_FILE_NAME = cp.get("OLD_WEIGHT_FILE_NAME");
	SERVER_PORT = cp.to_int(cp.get("SERVER_PORT"));
	SYNC_OLD_COUNT = cp.to_int(cp.get("SYNC_OLD_COUNT"));
	
	//cerr << strlen("140.113.86.124") << endl;
    //cerr << strlen(SERVER_IP) << endl;
}
