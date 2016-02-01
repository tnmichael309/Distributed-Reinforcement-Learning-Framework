#ifndef __SERVER_H__
#define __SERVER_H__

#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>


#include "transferStatus.h"

using namespace std;

class clientInfo{
public:
	void setInfo(string ip, int fd);
	int getFD();
	string getIP();
	
private:
	string clientIP;
	int clientFD;
};

class server
{
public:
	static int staticListenerD;
	static void handleShutdown(int sig);
	static void handleClientClosed(int sig);

	server();
	~server();
	void start(int port);

private:
	void error(const char *msg);
	int openListenerSocket();
	void bindToPort(int socket, int port);
	int catchSignal(int sig, void (*handler)(int));
	
	// handle gradient descent
	vector<clientInfo> m_clientInfos;
	int handleClient(int id);
	void initClient(int id);
	
	int m_descentCount;
	void incrementDescentCount();
	void updateClientWithNewWeights(int id);
	void notifyClientToReloadOldWeights(int id);
	void sendMessageToClient(int id, char command);
};

#endif