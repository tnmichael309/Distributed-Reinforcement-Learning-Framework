#include "server.h"

int server::staticListenerD = 0;

void clientInfo::setInfo(string ip, int fd){
	clientIP = ip;
	clientFD = fd;
}
int clientInfo::getFD(){
	return clientFD;
}
string clientInfo::getIP(){
	return clientIP;
}
	
void server::handleShutdown(int sig){
	if(staticListenerD)
		close(staticListenerD);
	exit(0);
}

void server::handleClientClosed(int sig){
	if(staticListenerD)
		close(staticListenerD);
}

server::server(){
	m_descentCount = -1;
}

server::~server(){;}

//
// Server start position
//
void server::start(int port)
{
	init_parameters();
	
	fd_set rfds; /* read file descriptor set */
	fd_set afds; /* active file descriptor set */
	int fd, nfds;
	
	// create listener socket
	if(catchSignal(SIGINT, server::handleShutdown) == -1)
		error("Can't set the interrupt handler");
	if(catchSignal(SIGPIPE, server::handleClientClosed) == -1)
		error("Can't set the interrupt handler");
	staticListenerD = openListenerSocket();

	bindToPort(staticListenerD, port);
	if(listen(staticListenerD, 200) < 0)
		error("Can't listen");
	
	nfds = getdtablesize();
	FD_ZERO(&afds);
	FD_SET(staticListenerD, &afds);
	
	struct sockaddr_in client_addr; /* the from address of a client*/
	unsigned int address_size = sizeof(client_addr);

	cout << "Start listen" << endl;
	/*int game_board[16] = {2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0};
	
	int best_move = game_ai_.find_best_move(game_board, 16);
	cout << best_move << endl;
	*/
	while(true) {
		memcpy(&rfds, &afds, sizeof(rfds));
		if (select(nfds, &rfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0) < 0)
			error("select error");
		if (FD_ISSET(staticListenerD, &rfds)) {
			int ssock;
			ssock = accept(staticListenerD, (struct sockaddr *)&client_addr, &address_size);
			if (ssock < 0)
				error("accept error");
			else{
				char clntIP[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &(client_addr.sin_addr),clntIP,sizeof(clntIP));
				printf("client IP is %s\n",clntIP);
				printf("client port is %d\n", (int) ntohs(client_addr.sin_port));
				
				// maintain client info
				clientInfo c;
				c.setInfo(clntIP, ssock);
				m_clientInfos.push_back(c);
				
				// init client
				initClient(m_clientInfos.size()-1);
	
				FD_SET(ssock, &afds);
			}
		}
	
		for(int i = 0; i < m_clientInfos.size(); i++){
		
			int fd = m_clientInfos[i].getFD();
			if (FD_ISSET(fd, &rfds)) {
				if(-1 == handleClient(i)){
					close(fd);
					FD_CLR(fd, &afds); 
					
					// delete client
					m_clientInfos.erase (m_clientInfos.begin()+i);
					break; 
				}
			}
		}
	}
}

void server::error(const char *msg)
{
	cerr << msg << ": " << strerror(errno) << endl;
	exit(1);
}

int server::openListenerSocket()
{
	struct protoent *ppe;
	if ((ppe = getprotobyname("tcp")) == 0) cerr << "can't get protocal entry" << endl;
	
	int s = socket(PF_INET, SOCK_STREAM, ppe->p_proto);
	if(s == -1)
		error("Can't open socket");
	return s;
}

void server::bindToPort(int socket, int port)
{
	struct sockaddr_in name;
	name.sin_family = AF_INET;
	name.sin_port = (in_port_t)htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	int reuse = 1;
	/*if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1)
		error("Can't set the reuse option on the socket");*/
	int c = bind(socket, (struct sockaddr *)&name, sizeof(name));
	if(c == -1)
		error("Can't bind to socket");
}

int server::catchSignal(int sig, void (*handler)(int))
{
	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	return sigaction(sig, &action, NULL);
}

int server::handleClient(int id)
{
	
	// update count
	incrementDescentCount();
	
	// do gradient descent
	char msg[1000];
	read(m_clientInfos[id].getFD(), msg, 1000);
	cout << "Server received: " << msg << endl;
	
	
	// save current weights
	
	
	// update newest weights and check whether to notify clients to change old weights
	updateClientWithNewWeights(id); // needs to be parallelized? pool of threads?
	notifyClientToReloadOldWeights(id);
	
	return 0;
}

void server::incrementDescentCount(){
	m_descentCount ++;
	m_descentCount = m_descentCount % SYNC_OLD_COUNT;
}

///
/// Notice the client that server is going to send new weights.
/// This function should be called after gradient decent is finished.
/// The transfer file part might has space to optimize (splice, tee)
///
void server::updateClientWithNewWeights(int id){
	// send new weights to client
	

	// copy to the first process in that IP
	sendMessageToClient(0, (char)(UPDATE_NEW + '0'));
	int read_fd;
	struct stat stat_buf;
	off_t offset = 0;
	
	read_fd = open (WEIGHT_FILE_NAME.c_str(), O_RDONLY); // Open the input file. 
	
	fstat (read_fd, &stat_buf); // Stat the input file to obtain its size.
	
	sendfile (m_clientInfos[0].getFD(), read_fd, &offset, stat_buf.st_size); // Blast the bytes from one file to the other.
	close (read_fd);
	sendMessageToClient(0, (char)(EOF));

	
	// send signal
	for(int i = 0; i < m_clientInfos.size(); i++){
		sendMessageToClient(i, (char)(RELOAD_NEW + '0'));
	}
}

///
/// Notice the client that server has done N gradient decents
/// client should copy current weights as old weights.
/// This function should be called after N gradient decents are finished.
///

void server::notifyClientToReloadOldWeights(int id){
	
	cerr << "current time-step: " << m_descentCount << endl;
	if(m_descentCount != 0) return;
	
	// notify clients to update old weights
	
	// send signal
	// the first process in that IP
	sendMessageToClient(0, (char)(COPY_OLD + '0'));
}	

void server::sendMessageToClient(int id, char command){
	int tempStdoutFd = dup(1);
	dup2(m_clientInfos[id].getFD(), 1);
	
	cout << command;
	fflush(stdout);
	
	cerr << "Server send command: " << command << endl;
	
	dup2(tempStdoutFd, 1);
	close(tempStdoutFd);
}

///
/// Tell the client to load the new weights.
/// If the client is the first process of the ip, then tell it to copy the weights as old.
///
void server::initClient(int id){
	sendMessageToClient(id, (char)(RELOAD_NEW + '0'));
	
	if(id == 0){ // the first process in that IP
		sendMessageToClient(id, (char)(COPY_OLD + '0'));
	}
}