#pragma once

#include <string>
#include <functional>
#include <map>

#include <cocos2d.h>

#include "Machine.h"


/*
	Абстрактный клиент
	С помощью него мы отправляем запрос на получение матриц
	Повышение, уменьшнения ставки
	Запуск рандомайзера
	Остановка рандомайзера
*/
class IClient : public cocos2d::Node
{
public:
	
	virtual ~IClient() 
	{}

	//Формат данных для спрайтов
	using BinnaryVector = std::vector<uint8_t>; //std::vector<std::byte>; 
	using BinnaryMap = std::map<std::string, BinnaryVector>;

	//иницилизация в стиле cocos2d
	virtual bool init() = 0;
	//Подключиться
	virtual bool connect(int timeout) = 0;
	//Проверить состояние подключения
	virtual bool isConnected() = 0;

	//Запустить барабан
	virtual bool spinStart() = 0;
	//Остановить барабан
	virtual bool spinStop() = 0;

	//Задать сиавку на сервере
	virtual bool setBet(unsigned int bet) = 0;
	//Увеличить ставку на сервере
	virtual bool addBet() = 0;
	//Удалить ставку на сервере
	virtual bool delBet() = 0;

	//Получить с сервера текущее состояние автомата
	virtual bool getMachine() = 0;

	//Скачивает с сервера бинарники, в виде структуры {имя : byte[]}
	virtual bool loadContentFiles() = 0;

	//Колбэк функция, возвращает новое состояние машины
	std::function<void(const Machine &machine)> onUpdateMachine = nullptr;
	//Колбэк обовещает, что аппарат останавливается
	std::function<void()> onSpinStop = nullptr;
	std::function<void()> onSpinStart = nullptr;
	//Колбэк возвращает выйгрыш (в принципе оно не нужно)
	std::function<void(int bet)> onWinner = nullptr;

	//не используеются, но если вдруг надо будет делать TCP клиент - они сгодятся
	std::function<void()> onDisconected = nullptr;
	std::function<void()> onConnected = nullptr;

	//повышение и понижение ставки
	std::function<void()> onAddBet = nullptr;
	std::function<void()> onDelBet = nullptr;

	//получение спрайтов с сервера
	std::function<void(const BinnaryMap &map)> onLoadContentFiles = nullptr;

	//задать GUID
	inline void setGUID(const std::string &g) { guid = g; }
	inline std::string getGUID() { return guid; }

protected:

	std::string guid;
	
};