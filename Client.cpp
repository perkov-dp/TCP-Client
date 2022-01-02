#include "Client.h"

/**
 * Конструетор Клиента.
 * Здесь происходит инициализация и настрока пар-ров соед-я Сервером.
 * Принимает на вход:
 * - семейство протоколов (AF_INET - интернета IPv4)
 * - type (SOCK_STREAM - потоковый сокет)
 * - пару сокетов <IP адрес, № порта>, однозначно устанавливающую пар-ры соединения
 */
Client::Client(pair<const string&, int> socket_pair) {
	/**
	 * Создание сокета TCP.
	 * Ф-я Socket возвращает дискриптор, к-рый идентифицирует сокет
	 * в последующих вызовах (connect, read)
	 */
	const int SOCKET_TYPE = SOCK_STREAM;	//	потоковый сокет
	const int PROTOCOL_FAMILY = AF_INET;	//	семейство протоколов IPv4
	socketFd = Socket(PROTOCOL_FAMILY, SOCKET_TYPE);

	/**
	 * Заполнение структуры адреса Интернета IP-адресом и № порта сервера
	 */
	string IP_address = socket_pair.first;
	int port_number = socket_pair.second;
	struct sockaddr_in servaddr = InitSockaddrStruct(PROTOCOL_FAMILY, IP_address, port_number);

	/**
	 * Установка соединения с сервером по протоколу TCP.
	 * Адрес сокета содержится в структуре servaddr.
	 */
	Connect(socketFd, servaddr);
}

/**
 * Создание сокета. Сокетом часто называют два значения,
 * идентифицирующие конечную точку: IP-адрес и номер порта.
 *
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
		perror("connect error");
		exit(EXIT_FAILURE);
	}
}

/**
 * Чтение n - байт по одному.
 * В сетевых программах могут быть случаи, когда за один раз не получится все прочитать.
 */
ssize_t Client::readn(int fd, void *vptr, size_t n) {
	size_t	nleft = 0;	//	оставшееся кол-во читаемых байт
	ssize_t	nread = 0;	//	кол-во считанных байт на данный мом-т
	char	*ptr;		//	ук-ль на читаемый буфер

	bzero(vptr, n);
	ptr = (char*)vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR) {
				/* and call read() again */
				continue;
			}
			return(-1);
		} else if (nread == 0) {
			break;	/* EOF */
		}

		nleft -= nread;	//	количество читаемых байт уменьшается на кол-во прочитанных байт
		ptr   += nread;	//	указатель сдвигается на кол-во прочитанных байт
	}
	return(n - nleft);		/* return >= 0 */
}

ssize_t Client::Readn(void *ptr, size_t nbytes) {
	ssize_t	n;
	if ((n = readn(socketFd, ptr, nbytes)) < 0) {
		perror("readn error");
	}

	return(n);
}

/**
 * Запись n байт.
 * Вариант для неблокирующего write, но годится и для неблокирующего
 */
ssize_t Client::writen(int fd, const void *vptr, size_t n) {
	size_t		nleft;		//	оставшееся кол-во записываемых байт
	ssize_t		nwritten;	//	кол-во записанных байт на данный мом-т
	const char	*ptr;		//	ук-ль на записываемый буфер

	ptr = (char*)vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) < 0) {
			if (errno == EINTR) {
				continue;		/* and call write() again */
			}
			return -1;			/* error */
		}

		nleft -= nwritten;	//	количество записываемых байт уменьшается на кол-во записанных байт
		ptr   += nwritten;	//	указатель сдвигается на кол-во записанных байт
	}
	return(n);
}

void Client::Writen(const void *ptr, size_t nbytes) {
	if (writen(socketFd, ptr, nbytes) != static_cast<int>(nbytes)) {
		perror("writen error");
	}
}

