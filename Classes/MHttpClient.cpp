#include "MHttpClient.h"
#include "nlohmann/json.hpp"

USING_NS_CC;

bool MHttpClient::init()
{
	client = network::HttpClient::getInstance();

	//TODO ��� ����� ������������ ������-���������� ��� ��������� GUID
	//��� ����������������� ������� � �� ���� ������ ���. ����������� ��� ����� win api
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
	��������� ������. �� ���� �� ������� json � ������������� ������ type � value
	������ ������� �������� value ����� ��������� ������� �� ���������
*/
void MHttpClient::request(const std::string &type, const std::string &value)
{
	using json = nlohmann::json;

	json j;

	j["type"] = type;
	j["value"] = value;
	j["GUID"] = guid; //������ ��� GUID

	std::string data = j.dump();

	network::HttpRequest* request = new network::HttpRequest();
	request->setRequestType(network::HttpRequest::Type::POST);
	request->setUrl(url);
	request->setHeaders({ "Content-Type: application/json", "GUID:" + guid });
	request->setResponseCallback(CC_CALLBACK_2(MHttpClient::answer, this));
	request->setRequestData(data.data(), data.size()); 
	//� ���������, ����� ���� ������������, ���� ������ � html ������� �������� ������ ����.
	//request->setTag(guid);
	client->send(request);

	request->release();
}

/*
	��� ����� ����� ����������� �� ����� �������� � ����������.
	�� ������ ��� �� ���������. �.�. ���������������� ������.
*/
void MHttpClient::answer(cocos2d::network::HttpClient * client, cocos2d::network::HttpResponse * response)
{

	using json = nlohmann::json;

	/*
		getTag ��� �� ��������, �.�. ��������� ������� �� ����� �� �������������
		������ ����. 
	*/
	//if (response->getHttpRequest()->getTag() != guid)
	//	return;


	/*
		�������� ���� ������
	*/
	auto data = response->getResponseData();
	if (data->empty())
	{
		CCLOG("responce data is empty");
		return;
	}

	std::string json_str(data->data(), data->size());
	
	json j;

	//� ����� ���������� �� ����������, �� nlohman json ������ � ���� � ��������.
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
		�������, ��� �� ��� GUID
	*/
	auto guid_resp = j.at("GUID").get<std::string>();
	if (guid != guid_resp)
		return;


	/*
		��������� ���� type
	*/
	if (j.find("type") != j.end())
	{
		type = j.at("type").get<std::string>();
	}

	if (type == "")
		return;
	

	/*
		��������� ��������� ������ ������ ����������
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
		���� ���� ������� ��������� �������.
		��� on* ������� ����������� ����� �������, ��������� �� ���
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
