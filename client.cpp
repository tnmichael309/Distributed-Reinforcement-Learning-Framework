#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sstream>
#include <vector>
#include <cstring>
#include <string.h>
#include <fcntl.h>
#include <ctime>
#include <iostream>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>

#include "transferStatus.h"

using namespace std;


#define MAX_MSG_LEN 1
#define FILE_BUF_LEN 4096

int readline(int fd, char *ptr, int maxlen)
{
	int n, rc;
	char c;
	*ptr = 0;
	for (n = 1; n < maxlen; n++)
	{
		if ((rc = recv(fd, &c, 1, 0)) == 1)
		{
			*ptr++ = c;
			if (c == EOF)
				break;
			
		}
		else if (rc == 0)
		{
			if (n == 1){
				cerr << "returned read line size: " << n << endl;
				return 0;
			}
			else
				break;
		}
		else {
			return -1;
		}
	}
	
	cerr << "returned read line size: " << n << endl;
	return n;
}

void handleCommand(int fd, int commandType){

	cerr << "receive command: " << commandType << endl;
	if(commandType == (int)COPY_OLD){
		int read_fd;
		int write_fd;
		struct stat stat_buf;
		off_t offset = 0;

		read_fd = open (WEIGHT_FILE_NAME.c_str(), O_RDONLY);
		fstat (read_fd, &stat_buf);
		write_fd = open (OLD_WEIGHT_FILE_NAME.c_str(), O_WRONLY | O_CREAT, 0666);
		
		sendfile (write_fd, read_fd, &offset, stat_buf.st_size);
		
		close (read_fd);
		close (write_fd);
		
	}else if(commandType == (int)RELOAD_NEW){
		
		int read_fd;
		read_fd = open (WEIGHT_FILE_NAME.c_str(), O_RDONLY);
		
		char fileBuffer[FILE_BUF_LEN] = {'\0'};
		int recvLen = 0;
		while((recvLen = read(read_fd, fileBuffer, FILE_BUF_LEN)) > 0){
			
			string sFile(fileBuffer);
			cerr << "Client reading file content: " << sFile << endl;
		}
		close(read_fd);
		
	}else if(commandType == (int)UPDATE_NEW){
	
		int write_fd;
		write_fd = open (WEIGHT_FILE_NAME.c_str(), O_WRONLY | O_CREAT, 0666);
		
		char fileBuffer[FILE_BUF_LEN] = {'\0'};
		int recvLen = 0;
		while((recvLen = readline(fd, fileBuffer, FILE_BUF_LEN)) >= 0){
			
			cerr << "Client received file: " << fileBuffer << endl;
			if(fileBuffer[recvLen-1] == EOF){
				write(write_fd, fileBuffer, recvLen-1);
				break;
			}else write(write_fd, fileBuffer, recvLen);
			
		}
		close(write_fd);
		//sleep(10);
		
	};
};

int main(){

	init_parameters();
	
	
	int                 unsend, len, i;
	int                 client_fd = -1;
	struct sockaddr_in  client_sin;
	struct hostent		*he;
	
	
	cerr << SERVER_IP << endl;
	cerr << SERVER_PORT << endl;
	
	// initialize this server info
	he = gethostbyname(SERVER_IP.c_str());
	client_fd = socket(AF_INET,SOCK_STREAM,0);
	
	memset(&client_sin, 0, sizeof(client_sin)); 
	client_sin.sin_family = AF_INET;
	client_sin.sin_addr = *((struct in_addr *)he->h_addr); 
	client_sin.sin_port = htons(SERVER_PORT);
	

	
	// set non-blocking
	int maxfd = 0;
		
	// connect to server now
	if(client_fd == -1) return -1;
		
	// setting non-blocking socket
	int flags;
	if (-1 == (flags = fcntl(client_fd, F_GETFL, 0)))
		flags = 0;
	fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
	
	// find max fd
	if(client_fd > maxfd) maxfd = client_fd;
	
	// keep connecting sockfd
	while(1){
		if(connect(client_fd,(struct sockaddr *)&client_sin,sizeof(client_sin)) == -1) {
			//cerr << "connect fail for user#: " << i << "\n";
		}else{
			//cerr << "connect success for user#: " << i << "\n";
			break; // connect successfully
		}
	}	
	
	
	char msg[1];
	while(1){
		// read msg from server
		int recvLen = read(client_fd,msg,MAX_MSG_LEN);
		if(recvLen > 0){
			cerr << "receive msg: " << msg << endl;
			handleCommand(client_fd, (int)(msg[0]-'0'));
		}
		
		// DQN
		
		
		// write gradient to server
		int randomSleepTime = rand()%1000000;
		usleep(randomSleepTime);
		
		write(client_fd, "descent!", strlen("descent!")+1);
	}
	
	return 0;
}