#include "Client.h"

/**
 * Конструетор Клиента.
 * Здесь происходит инициализация и настрока пар-ров соед-я Сервером.
 * Принимает на вход:
 * - семейство протоколов (AF_INET - интернета IPv4)
 * - type (SOCK_STREAM - потоковый сокет)
 * - пару сокетов <IP адрес, № порта>, однозначно устанавливающую пар-ры соединения
 */
Client::Client(int socket_type, pair<const string&, int> socket_pair) {
	const int PROTOCOL_FAMILY = AF_INET;	//	семейство протоколов IPv4
	socketFd = Socket(PROTOCOL_FAMILY, socket_type);

	string IP_address = socket_pair.first;
	int port_number = socket_pair.second;
	struct sockaddr_in servaddr = InitSockaddrStruct(PROTOCOL_FAMILY, IP_address, port_number);
	Connect(socketFd, servaddr);
}

/**
 * Создание сокета.
 * Принимает на вход:
 * - семейство протоколов (AF_INET - интернета IPv4)
 * - type (SOCK_STREAM - потоковый сокет)
 * - protocol - обычно 0
 * Все вместе - это название TCP-сокета
 * Возвращает дескриптор сокета
 */
int Client::Socket(int family, int type) {
	int sockfd = 0;
	if ((sockfd = socket(family, type, 0)) < 0) {
		perror("Socket() error");
		exit(EXIT_FAILURE);
	}

	return sockfd;
}

/**
 * Преобразование текстового IP-адреса в требуемое представление
 */
int Client::Inet_pton(int family, const string& ip_address, struct sockaddr_in& servaddr) {
	int rval = 0;
	if ((rval = inet_pton(family, ip_address.c_str(), &servaddr.sin_addr)) == 0) {
		cout << "Inet_pton() Invalid address " << ip_address << endl;
		exit(EXIT_FAILURE);
	} else if (rval == -1) {
		perror("Inet_pton() error");
		exit(EXIT_FAILURE);
	}

	return rval;
}

/**
 * Заполнение структуры адреса Интернета IP-адресом и № порта сервера
 */
sockaddr_in Client::InitSockaddrStruct(int family, const string& ip_address, uint16_t port_number) {
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = family;	//	IPv4
	servaddr.sin_port = htons(port_number);	//	приводит № порта к нужному формату

	//	Inet_pton - преобразовывает строковый IP-адрес в двоичный формат
	Inet_pton(family, ip_address, servaddr);

	return servaddr;
}

/**
 * Установка соединения с сервером
 */
void Client::Connect(int sockfd, const struct sockaddr_in& servaddr) {
	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		cout << "connect error" << endl;
		exit(EXIT_FAILURE);
	}
}
