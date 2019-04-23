#pragma once
#include "includes.h"

class ClientChecker
{
private:
	//Адрес сервера
	std::string host;
	//Порт сервера
	std::string port;
	void initWSA(WSADATA& wsaData);
	SOCKET initClientSocket();
	void write(SOCKET& client, const std::string& toSend);
	void read(SOCKET& client, std::string& toFill);
	//Функция, которую надо определить при наследовании
	virtual void handle(const std::string& toHandle) = 0;
	void close(SOCKET& client);
public:
	ClientChecker(std::string host, std::string port);
	~ClientChecker() = default;
	void execute(const std::string& key, const std::string& user);
};

