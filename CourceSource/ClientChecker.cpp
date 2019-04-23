#include "ClientChecker.h"

//Размер буфера
const int bufferSize = 512;

//Инициализация Socket API
void ClientChecker::initWSA(WSADATA& wsaData)
{
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		perror("WSA Startup failed: ");
		exit(1);
	}
}

//Создание подключения
SOCKET ClientChecker::initClientSocket()
{
	struct addrinfo* result = nullptr, info;
	ZeroMemory(&info, sizeof(info));
	//Выбор протокола
	info.ai_family = AF_UNSPEC;
	//Выбор типа подключения
	info.ai_socktype = SOCK_STREAM;
	//Используется TCP/IP протокол
	info.ai_protocol = IPPROTO_TCP;

	//Получение данных о подключении
	int iResult = getaddrinfo(host.c_str(), port.c_str(), &info, &result);
	if (iResult != 0) {
		perror("Getting of addrinfo failed: ");
		WSACleanup();
		exit(2);
	}

	SOCKET toReturn = INVALID_SOCKET;
	
	//Перебираем возможные подключения
	for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
		//Пробуем создать подключение
		toReturn = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (toReturn == INVALID_SOCKET) {
			perror("Socket init failed: ");
			WSACleanup();
			exit(3);
		}

		//Пробуем связаться с сервером
		iResult = connect(toReturn, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(toReturn);
			toReturn = INVALID_SOCKET;
			continue;
		}
		break;
	}

	if (toReturn == INVALID_SOCKET) {
		perror("Socket connect failed: ");
		WSACleanup();
		exit(4);
	}

	freeaddrinfo(result);
	puts("Connection opened...");
	fflush(stdout);
	return toReturn;
}

//Отправка строки в установленное подключение
void ClientChecker::write(SOCKET& client, const std::string& toSend)
{
	int iResult = send(client, toSend.c_str(), toSend.size(), 0);
	if (iResult == SOCKET_ERROR) {
		perror("Sending failed: ");
		closesocket(client);
		WSACleanup();
		exit(5);
	}
}

//Получение ответа от сервера
void ClientChecker::read(SOCKET& client, std::string& toFill)
{
	int iResult;
	//Инициализация буфера
	char buf[bufferSize + 1];
	toFill.clear();
	do {
		ZeroMemory(buf, sizeof(buf));
		//Получаем информацию
		iResult = recv(client, buf, bufferSize, 0);
		//Если она получена
		if (iResult > 0) {
			toFill += buf;
		}
		else {
			perror("Receiving failed: ");
			closesocket(client);
			WSACleanup();
			exit(6);
		}
		//Пока информация осталась
	} while (iResult == bufferSize);
}

//Обрываем подключение
void ClientChecker::close(SOCKET & client)
{
	puts("Connection closed...");
	//Закрываем подключение
	closesocket(client);
	//Отключаем Socket API
	WSACleanup();
	fflush(stdout);
}

//Конструктор
ClientChecker::ClientChecker(std::string host, std::string port)
	: host(host), port(port) {}

//Метод запуска запроса
void ClientChecker::execute(const std::string & key, const std::string & user)
{
	WSADATA wsaData;
	initWSA(wsaData);

	SOCKET client = initClientSocket();

	//Создание JSON объекта с данными
	std::string data = "{ \"key\": \"";
	data += key;
	data += "\", \"user\": \"";
	data += user;
	data += "\" }";

	write(client, data);
	read(client, data);
	//Управление ответом
	handle(data);
	close(client);
}
