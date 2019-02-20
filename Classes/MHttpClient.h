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
		������������ ������������� � ����� cocos2d
	*/
	virtual bool init() override;

	/*
		������ URL, � �������� ����� ����������� ����������� (������ URL ������ �������� � AppDelegate)
	*/
	inline void setURL(const std::string &str)
	{
		url = str;
	}

	//��� ������� ��� �� �����
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
	//��� url
	std::string url;
	//�������� ������� �� ������
	void request(const std::string &type, const std::string &value );

	//��������� ������ �� �������
	void answer(cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response);

	//��� http ������ ������
	cocos2d::network::HttpClient *client;

	bool requestSpin;
	bool waitMachine;
};