#pragma once
#include "includes.h"

class ServerChecker
{
private:
	//Порт сервера
	std::string port;
	//Словарь с лицензионными ключами и пользователями
	std::map<std::string, std::string> keyDB;
	void initWSA(WSADATA& wsaData);
	SOCKET initServerSocket();
	void startListening(SOCKET& listener);
	SOCKET acceptNewClient(SOCKET& listener);
	void read(SOCKET& client, std::string& input);
	void write(SOCKET& client, const std::string& toSend);
	void close(SOCKET& client);
	void validate(std::string& data);
	//Функции, которую надо определить при наследовании
	virtual std::map<std::string, std::string> fillDB() = 0;
	virtual std::string ok() = 0;
	virtual std::string deny() = 0;
	virtual std::string bad() = 0;
public:
	ServerChecker(std::string port);
	~ServerChecker() = default;
	void start();
};
