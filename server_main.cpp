#include "server.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]){

	server s;
	s.start(atoi(argv[1]));
	
	return 0;
}