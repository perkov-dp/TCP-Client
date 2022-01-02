#pragma once
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
using namespace std;

#include <sys/types.h>
#include <unistd.h>	//	read
#include <netinet/in.h>	//	sockaddr_in
#include <sys/socket.h>	//	socket
#include <arpa/inet.h>	//	inet_pton

class Client {
public:
	Client(pair<const string&, int> socket_pair);
	int socketFd;
private:
	int Socket(int family, int socket_type);
	int Inet_pton(int family, const string& ip_address, struct sockaddr_in& servaddr);
	sockaddr_in InitSockaddrStruct(int family, const string& ip_address, uint16_t port_number);
	void Connect(int sockfd, const struct sockaddr_in& servaddr);
};
