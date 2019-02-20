#pragma once

#include "Client.h"
#include "Machine.h"

#include <cocos2d.h>
#include <network/HttpClient.h>

/*
	Http client 
*/
class MHttpClient : public IClient
{
public:

	/*
		Классическая инициализация в стиле cocos2d
	*/
	virtual bool init() override;

	/*
		задаем URL, к которому будет происходить подключение (Сейчас URL жестко прописан в AppDelegate)
	*/
	inline void setURL(const std::string &str)
	{
		url = str;
	}

	//эти функции тут не нужны
	virtual bool connect(int timeout) override;
	virtual bool isConnected() override;

	
	virtual bool spinStart() override;
	virtual bool spinStop() override;

	virtual bool setBet(unsigned int bet) override;
	virtual bool addBet() override;
	virtual bool delBet() override;

	virtual bool getMachine() override;

	virtual bool loadContentFiles() override;

	CREATE_FUNC(MHttpClient);

private:
	//наш url
	std::string url;
	//отправка запроса на сервер
	void request(const std::string &type, const std::string &value );

	//получение ответа от сервера
	void answer(cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response);

	//сам http клиент кокоса
	cocos2d::network::HttpClient *client;

	bool requestSpin;
	bool waitMachine;
};