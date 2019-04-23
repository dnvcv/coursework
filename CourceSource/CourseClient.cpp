#include "ClientChecker.h"

//Наследуем наш класс от библиотечного
class MyClientChecker : public ClientChecker {
private:
	//Переопределяем управление ответом
	void handle(const std::string& toHandle) {
		//Выводим информацию в основной поток
		puts(toHandle.c_str());
		fflush(stdout);
	}
public:
	//Переопределяем конструктор
	MyClientChecker(std::string host, std::string port)
		: ClientChecker(host, port) {}
};

//В аргументах надо передать адрес сервера и порт если он не равен 80
int main(int argc, char** argv) {
	if (argc == 1 || argc > 3) {
		printf("Usage: %s host [port=80]\n", argv[0]);
		return 0;
	}
	std::string host = argv[1];
	std::string port = argc == 3 ? argv[2] : "80";

	//Создаём объект
	MyClientChecker checker(host, port);

	std::string user, key;
	std::cout << "Enter username: " << std::endl;
	//Считываем имя пользователя
	std::getline(std::cin, user);
	std::cout << "Enter license key: " << std::endl;
	//Считываем лицензионный ключ
	std::getline(std::cin, key);
	//Отправляем запрос на сервер
	checker.execute(key, user);
	return 0;
}