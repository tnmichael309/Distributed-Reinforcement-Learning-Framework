#ifndef __TRANSFERSTAT_H__
#define __TRANSFERSTAT_H__

#include <iostream>
using namespace std;

enum COMMAND{
	COPY_OLD,
	RELOAD_NEW,
	UPDATE_NEW
};

extern string SERVER_IP;
extern int SERVER_PORT;
extern int SYNC_OLD_COUNT;
extern string WEIGHT_FILE_NAME;
extern string OLD_WEIGHT_FILE_NAME;

void init_parameters();

#endif