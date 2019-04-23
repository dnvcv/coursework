#include "ServerChecker.h"

//Наследуем наш класс от библиотечного
class MyServerChecker : public ServerChecker {
private:
	//Переопределяем метод заполнения словаря с лицензионными ключами
	std::map<std::string, std::string> fillDB() {
		std::map<std::string, std::string> toReturn;
		toReturn.insert({ "123kkk54l3", "Vasya" });
		return toReturn;
	}
	//Переопределяем сообщение при найденном совпадении
	std::string ok() {
		return "Valid license";
	}
	//Переопределяем сообщение при некорректном запросе
	std::string bad() {
		return "Invalid request";
	}
	//Переопределяем сообщение при отсутствии совпадения
	std::string deny() {
		return "Invalid license";
	}
public:
	//Переопределяем конструктор
	MyServerChecker(std::string port)
		: ServerChecker(port) {}
};

//В аргументах надо передать порт если он не равен 80
int main(int argc, char** argv) {
	if (argc == 1) {
		//Создаём на стандартном порту объект сервера
		MyServerChecker checker("80");
		//Запуск
		checker.start();
	}
	else if (argc == 2) {
		//Создаём на определённом порту объект сервера
		MyServerChecker checker((std::string)argv[1]);
		//Запуск
		checker.start();
	}
	else {
		printf("Usage: %s [port=80]\n", argv[0]);
	}
	return 0;
}