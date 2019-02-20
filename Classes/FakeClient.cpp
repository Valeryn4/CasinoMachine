#include "FakeClient.h"
#include <cocos2d.h>

USING_NS_CC;

bool FakeClient::init()
{
	return true;
}

bool FakeClient::connect(int timeout)
{
	return true;
}

bool FakeClient::isConnected()
{
	return true;
}

bool FakeClient::spinStart()
{
	if (machine.getStatus() == Machine::Status::Spin)
		return false;
	machine.setStatus(Machine::Status::Spin);
	//machine.setTotalBet(0);

	updateMachine();

	if (machine.getTotalBet() <= 0)
	{
		if (onSpinStop)
			onSpinStop();
		return false;
	}
	
	if (onSpinStart)
		onSpinStart();

	return true;
}

bool FakeClient::spinStop()
{
	if (machine.getStatus() == Machine::Status::Stop)
		return false;
	machine.setStatus(Machine::Status::Stop);
	machine.randomize();
	auto res = machine.resultWinner();

	updateMachine();

	if (onSpinStop)
		onSpinStop();

	if (onWinner)
		onWinner(res);

	return true;
}

bool FakeClient::setBet(unsigned int bet_)
{
	machine.setTotalBet(bet_);
	updateMachine();
	
	return true;
}

bool FakeClient::addBet()
{
	machine.setTotalBet(machine.getTotalBet() + 10);
	updateMachine();
	if (onAddBet)
		onAddBet();
	return true;
}

bool FakeClient::delBet()
{
	int bet = machine.getTotalBet();
	if (bet <= 9)
		return false;
	machine.setTotalBet(bet - 10);
	updateMachine();
	if (onDelBet)
		onDelBet();
	return true;
}

bool FakeClient::getMachine()
{
	if (onUpdateMachine)
		onUpdateMachine(machine);

	return true;
}

bool FakeClient::loadContentFiles()
{
	auto fileUtils = FileUtils::getInstance();
	BinnaryMap mapFiles;
	
	const static std::vector<std::string> symbolList = {
		"1.png",
		"2.png",
		"3.png",
		"4.png",
		"5.png",
		"6.png",
		"7.png",
		"8.png",
		"9.png",
		"10.png"
	};

	const static std::vector<std::string> partsList = {
		"back.png",
		"minus.png",
		"minus_press.png",
		"plus.png",
		"plus_press.png",
		"reel_shadow.png",
		"spin.png",
		"spin_press.png"
	};

	for (auto &&name : symbolList)
	{
		std::string pathFile = "Symbols/" + name;
		std::vector<uint8_t> data;
		fileUtils->getContents("TestLoad/" + pathFile, &data);
		mapFiles.emplace(pathFile, std::move(data));
	}
	
	for (auto &&name : partsList)
	{
		std::string pathFile = "Parts/" + name;
		std::vector<uint8_t> data;
		fileUtils->getContents("TestLoad/" + pathFile, &data);
		mapFiles.emplace(pathFile, std::move(data));
	}

	if (onLoadContentFiles)
		onLoadContentFiles(mapFiles);

	return true;
}

void FakeClient::updateMachine()
{
	if (onUpdateMachine != nullptr)
		onUpdateMachine(machine);
}
