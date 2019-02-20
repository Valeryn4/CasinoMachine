#include "MHttpClient.h"
#include "nlohmann/json.hpp"

USING_NS_CC;

bool MHttpClient::init()
{
	client = network::HttpClient::getInstance();

	//TODO тут нужно использовать крипто-библиотеку для генерации GUID
	//для демонстрационного проекта я не стал тянуть доп. зависимости или юзать win api
	static const std::string symbols =
	{
		"0123456789qwertyuiopasdfghjklzxcvbnm_-?@*(@){}<>/?\!QWERTYUIOPASDFGHJKLZXCVBNM"
	};

	this->guid = {};
	for (int i = 0; i < 10; i++)
	{
		this->guid.push_back(symbols.at(rand() % symbols.length()));
	}
	//END TODO
	
	return true;
}

bool MHttpClient::connect(int timeout)
{
	return true;
}

bool MHttpClient::isConnected()
{
	return true;
}

bool MHttpClient::spinStart()
{
	if (requestSpin)
		return false;

	request("SpinStart", "");
	return true;
}

bool MHttpClient::spinStop()
{
	if (requestSpin)
		return false;

	request("SpinStop", "");
	return true;
}

bool MHttpClient::setBet(unsigned int bet)
{
	
	request("SetBet", std::to_string(bet));
	return true;
}

bool MHttpClient::addBet()
{
	request("AddBet", "");
	return true;
}

bool MHttpClient::delBet()
{
	request("DelBet", "");
	return true;
}

bool MHttpClient::getMachine()
{
	request("Machine", "");
	return true;
}

bool MHttpClient::loadContentFiles()
{
	request("LoadContent", "");
	return true;
}

/*
	Формируем запрос. По сути мы создаем json с обязательными полями type и value
	сейчас серверу значение value кроме некоторых случаев не интересно
*/
void MHttpClient::request(const std::string &type, const std::string &value)
{
	using json = nlohmann::json;

	json j;

	j["type"] = type;
	j["value"] = value;
	j["GUID"] = guid; //задаем наш GUID

	std::string data = j.dump();

	network::HttpRequest* request = new network::HttpRequest();
	request->setRequestType(network::HttpRequest::Type::POST);
	request->setUrl(url);
	request->setHeaders({ "Content-Type: application/json", "GUID:" + guid });
	request->setResponseCallback(CC_CALLBACK_2(MHttpClient::answer, this));
	request->setRequestData(data.data(), data.size()); 
	//к сожалению, нигде нету документации, куда именно в html запросе пихается данное поле.
	//request->setTag(guid);
	client->send(request);

	request->release();
}

/*
	Эту часть можно реализовать на много красивее и лаконичнее.
	Но сейчас это не актуально. Т.к. демонстрационный проект.
*/
void MHttpClient::answer(cocos2d::network::HttpClient * client, cocos2d::network::HttpResponse * response)
{

	using json = nlohmann::json;

	/*
		getTag тут не работает, т.к. серверная сторона не знает об существовании
		такого поля. 
	*/
	//if (response->getHttpRequest()->getTag() != guid)
	//	return;


	/*
		получаем тело ответа
	*/
	auto data = response->getResponseData();
	if (data->empty())
	{
		CCLOG("responce data is empty");
		return;
	}

	std::string json_str(data->data(), data->size());
	
	json j;

	//в играх исключение не желательны, но nlohman json только с ними и работает.
	try
	{
		j = json::parse(json_str);
	}
	catch (json::exception &e)
	{
		CCLOG("responce data parce error! err %s", e.what());
		return;
	}

	std::string type = "";

	/*
		сверяем, наш ли это GUID
	*/
	auto guid_resp = j.at("GUID").get<std::string>();
	if (guid != guid_resp)
		return;


	/*
		проверяем поле type
	*/
	if (j.find("type") != j.end())
	{
		type = j.at("type").get<std::string>();
	}

	if (type == "")
		return;
	

	/*
		обновляем состояние машины данным замыканием
	*/
	auto updateMachine = [&]()
	{
		if (j.find("Machine") != j.end())
		{
			//auto machineJson = j.at("Machine");
			auto newMachine = Machine::machineFromJSON(j["Machine"]);
			if (onUpdateMachine)
				onUpdateMachine(newMachine);
		}
	};

	/*
		ниже идет перечнь обработки ответов.
		все on* функции проверяются перед вызовом, назначины ли они
	*/
	if (type == "Machine")
	{
		if (!j.at("value").get<bool>())
			return;

		updateMachine();
	}
	else if (type == "SpinStart")
	{
		if (!j.at("value").get<bool>())
			return;

		updateMachine();

		if (onSpinStart)
			onSpinStart();
	}
	else if (type == "SpinStop")
	{
		if (!j.at("value").get<bool>())
			return;

		if (j.find("Machine") != j.end())
		{
			
			auto newMachine = Machine::machineFromJSON(j["Machine"]);
			if (onUpdateMachine)
				onUpdateMachine(newMachine);

			if (newMachine.getWinner() > 0)
				if (onWinner)
					onWinner(newMachine.getWinner());
		}

		if (onSpinStop)
			onSpinStop();
	}
	else if (type == "AddBet")
	{
		if (!j.at("value").get<bool>())
			return;

		updateMachine();

		if (onAddBet)
			onAddBet();
	}
	else if (type == "DelBet")
	{
		if (!j.at("value").get<bool>())
			return;

		updateMachine();

		if (onDelBet)
			onDelBet();
	}
	else if (type == "SetBet")
	{
		if (!j.at("value").get<bool>())
			return;

		updateMachine();

		
	}
	else if (type == "LoadContent")
	{
		if (!j.at("value").get<bool>())
			return;

		auto bin = j.at("data").get<BinnaryMap>();

		if (onLoadContentFiles)
			onLoadContentFiles(bin);
	}
	
}
