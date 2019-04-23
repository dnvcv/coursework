#include "ServerChecker.h"

//Размер буфера
const int bufferSize = 512;

//Конструктор
ServerChecker::ServerChecker(std::string port)
	: port(port) {}

//Метод запуска сервера
void ServerChecker::start()
{
	//Инициализация словаря
	keyDB = fillDB();
	
	WSADATA wsaData;
	initWSA(wsaData);
	
	SOCKET listener = initServerSocket();

	startListening(listener);

	//Непрерывно принимаем запросы
	while (true) {
		SOCKET client = acceptNewClient(listener);
		std::string clientInfo;
		read(client, clientInfo);
		validate(clientInfo);
		write(client, clientInfo);
		close(client);
	}

	//Закрываем порт
	closesocket(listener);
	//Отключаем Socket API
	WSACleanup();
}

//Инициализация Socket API
void ServerChecker::initWSA(WSADATA& wsaData)
{
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		perror("WSA Startup failed: ");
		exit(1);
	}
}

//Открытие серверного порта
SOCKET ServerChecker::initServerSocket()
{
	struct addrinfo* result = nullptr, info;
	ZeroMemory(&info, sizeof(info));
	//Устанавливаем IPv4
	info.ai_family = AF_INET;
	//Выбор типа подключения
	info.ai_socktype = SOCK_STREAM;
	//Используем TCP/IP протокол
	info.ai_protocol = IPPROTO_TCP;
	//Автоопределение
	info.ai_flags = AI_PASSIVE;

	//Получение данных о серверном порте
	int iResult = getaddrinfo(nullptr, port.c_str(), &info, &result);
	if (iResult != 0) {
		perror("Getting of standart addrinfo failed: ");
		WSACleanup();
		exit(2);
	}

	SOCKET toReturn = INVALID_SOCKET;
	//Инициализация серверного порта
	toReturn = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (toReturn == INVALID_SOCKET) {
		perror("Server socket init failed: ");
		WSACleanup();
		exit(3);
	}

	//Привязка порта к адресу
	iResult = bind(toReturn, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		perror("Binding failed: ");
		freeaddrinfo(result);
		closesocket(toReturn);
		WSACleanup();
		exit(4);
	}

	freeaddrinfo(result);
	puts("Server started...");
	fflush(stdout);
	return toReturn;
}

//Начинаем слушать запросы на порт
void ServerChecker::startListening(SOCKET& listener)
{
	if (listen(listener, SOMAXCONN) == SOCKET_ERROR) {
		perror("Listening failed: ");
		closesocket(listener);
		WSACleanup();
		exit(5);
	}
}

//Устанавливаем подключение с клиентом
SOCKET ServerChecker::acceptNewClient(SOCKET& listener)
{
	SOCKET toReturn = INVALID_SOCKET;

	toReturn = accept(listener, nullptr, nullptr);
	if (toReturn == INVALID_SOCKET) {
		perror("Accepting failed: ");
		closesocket(listener);
		WSACleanup();
		exit(6);
	}
	puts("Connection opened...");
	fflush(stdout);
	return toReturn;
}

//Считываем содержимое запроса
void ServerChecker::read(SOCKET& client, std::string& input)
{
	int iResult;
	//Инициализация буфера
	char buf[bufferSize + 1];
	do {
		ZeroMemory(buf, sizeof(buf));
		//Получаем информацию
		iResult = recv(client, buf, bufferSize, 0);
		//Если она получена
		if (iResult > 0) {
			input += buf;
		}
		else {
			perror("Receiving failed: ");
			closesocket(client);
			WSACleanup();
			exit(7);
		}
		//Пока информация осталась
	} while (iResult == bufferSize);
}

//Отправка ответа
void ServerChecker::write(SOCKET & client, const std::string & toSend)
{
	int iResult = send(client, toSend.c_str(), toSend.size(), 0);
	if (iResult == SOCKET_ERROR) {
		perror("Sending failed: ");
		closesocket(client);
		WSACleanup();
		exit(8);
	}
}

//Обрываем подключение клиента
void ServerChecker::close(SOCKET & client)
{
	puts("Connection closing...");
	//Закрываем подключение
	int iResult = shutdown(client, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		perror("Shutdown failed: ");
		closesocket(client);
		WSACleanup();
		exit(8);
	}
	//Удаляем информацию о нём
	closesocket(client);
	fflush(stdout);
}

//Проводим валидацию данных
void ServerChecker::validate(std::string & data)
{
	//Лог
	printf("Login attempt with: %s\n", data.c_str());
	fflush(stdout);
	//Объект с содержимым от регулярного выражения
	std::smatch matches;
	//Если JSON корректен
	if (std::regex_match(data, matches,
		std::regex(R"(\s*\{\s*"key"\s*:\s*"(\w+)\"\s*,\s*"user"\s*:\s*"(\w+)\"\s*\}\s*)"))) {
		//Получаем содержимое ключа
		std::string key = matches[1];
		//Получаем содержимое имени пользователя
		std::string user = matches[2];
		//Если существует лицензионный ключ и он принадлежит пользователю
		if (keyDB.count(key) && keyDB[key] == user) {
			//Корректный ответ
			data = ok();
		}
		else {
			//Некорректный ответ
			data = deny();
		}
	}
	//То же самое, только поля идут наоборот
	else if (std::regex_match(data, matches,
		std::regex(R"(\s*\{\s*"user"\s*:\s*"(\w+)\"\s*,\s*"key"\s*:\s*"(\w+)\"\s*\}\s*)"))) {
		std::string key = matches[2];
		std::string user = matches[1];
		if (keyDB.count(key) && keyDB[key] == user) {
			data = ok();
		}
		else {
			data = deny();
		}
	}
	//Некорректный запрос
	else {
		data = bad();
	}
	fflush(stdout);
}
