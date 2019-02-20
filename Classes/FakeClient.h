#pragma once

#include "Client.h"

/*
	Фейковый клиент
	Нужен для отладки и тестирования.
	По сути это эмуляция http клиента
*/
class FakeClient : public IClient
{
public:

	virtual bool init() override;

	virtual bool connect(int timeout) override;
	virtual bool isConnected() override;

	virtual bool spinStart() override;
	virtual bool spinStop() override;

	virtual bool setBet(unsigned int bet) override;
	virtual bool addBet() override;
	virtual bool delBet() override;

	virtual bool getMachine() override;
	//загружает из папки TestLoad/ бинарники и передает клиенту
	virtual bool loadContentFiles() override;

	CREATE_FUNC(FakeClient);

private:

	void updateMachine();
	Machine machine;

};