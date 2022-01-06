#include "Client.h"
#include <vector>

int main(int argc, char *argv[]) {
	string IP_ADDRESS = "127.0.0.1";
	const uint16_t PORT_NUMBER = 34543;	//	сервер даты и времени
	pair<string&, int> socket_pair(IP_ADDRESS, PORT_NUMBER);

	vector<Client> clients(5);
	for (size_t i = 0; i < clients.size(); i++) {
		clients.at(i).ClientInit(socket_pair);
	}

	/**
	 * Чтение и обработка ответа сервера.
	 * Ответ может быть дан не за один раз,
	 * поэтому read вызывается циклически.
	 * Соединение прерывается в следующих случаях:
	 * - сервер разорвал соединение и read вернула 0
	 * - при ошибке
	 */
	//clients.at(0).str_cli(stdin);

	exit(0);
}
