#include "requesthandler.h"

#include <nlohmann/json.hpp>

#include <iostream>

RequestHandler::~RequestHandler()
{

}

void RequestHandler::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{

    using json = nlohmann::json;

    if (request.getMethod() != "POST")
    {
        qDebug() << "methond no POST";
        return;
    }

    if (request.getHeader("Content-Type") != QByteArray("application/json"))
    {
        qDebug() << "Header content invalid: " << QString(request.getHeader("Content-Type"));
        return;
    }

    auto body = request.getBody();

    std::string type;
    std::string value;
    QString guid;

    response.setHeader("Content-Type", "application/json");
    response.setStatus(200);

    try
    {
        auto str_json = body.toStdString();
        str_json.erase(std::remove(str_json.begin(), str_json.end(), '\\'),
                       str_json.end());

        auto j = json::parse(str_json);
        std::cout << "json parse. \n\tstring:" << str_json << "\n\tjson" << j.dump() << std::endl;

        guid = QString::fromStdString( j["GUID"].get<std::string>() );

        if (!m_clients.contains(guid))
        {
            m_clients.insert(guid, Machine());
            qDebug() << "add new client! guid: " << guid;
        }


        type = j["type"].get<std::string>();
        qDebug() << "request type: " << QString::fromStdString(type);

        value = j["value"].get<std::string>();
        qDebug() << "request value: " << QString::fromStdString(value);

        processResponce(guid, type, value, response);

    }
    catch (json::parse_error &e)
    {
        qWarning() << "json parse failed! err:" << e.what();
        return;
    }
    catch (json::exception &e)
    {
        qWarning() << "json exception! err:" << e.what();
        return;
    }
}

void RequestHandler::init()
{

    Q_INIT_RESOURCE(res_sprites);

    //загружает спрайты

#ifdef QT_DEBUG
    QDirIterator it(":", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        qDebug() << it.next();

    }
#endif

    auto loadFiles = [this](const QString &subDir)
    {
        QDirIterator it(/*":/Parts"*/ subDir, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            auto name = it.next();
            auto key = name.remove(":/");
            auto file = QFile(":/" + name);

            qDebug() << "Load file name:" << name << " key:" << key;

            if (!file.open(QIODevice::ReadOnly))
            {
                qCritical() << "\tload failed! err: " << file.errorString();
                continue;
            }
            QByteArray blob = file.readAll();
            BinnaryVector bin(blob.begin(), blob.end());

            this->m_fileMap.emplace(key.toStdString(), bin);

            file.close();
            qDebug() << "\tloaded butes: " << bin.size();

        }
    };

    loadFiles(":/Parts");
    loadFiles(":/Symbols");
}

void RequestHandler::processResponce(const QString &guid, const std::string &type, const std::string &value,
                                      stefanfrings::HttpResponse &responce)
{
    if (type == "")
            return;

    using json = nlohmann::json;

    json answer_json;

    const std::string jtype = "type";
    const std::string jvalue = "value";
    const std::string jmachine = "Machine";

    qDebug() << "set type and falue";
    answer_json[jtype] = type;
    answer_json[jvalue] = false;
    answer_json["GUID"] = guid.toStdString();

    Machine& machine = m_clients[guid];


    auto writeMachine = [&]()
    {

        auto machineJson = Machine::machineToJSON(machine);
        answer_json[jmachine] = machineJson;
        qDebug() << ">> machine ID " << guid << " update";
        if (type != "LoadContent") //не стоит выводить все бинарники
            qDebug() << "answer json: \n" << answer_json.dump().c_str();

    };


    if (type == "Machine")
    {

        answer_json[jvalue] = true;
        writeMachine();

    }
    else if (type == "SpinStart")
    {
        if (machine.getStatus() == Machine::Status::Stop && machine.getTotalBet() > 0)
        {
            answer_json[jvalue] = true;
            machine.setWinner(0);
            machine.setStatus(Machine::Status::Spin);
            writeMachine();
        }
    }
    else if (type == "SpinStop")
    {
        if (machine.getStatus() == Machine::Status::Spin)
        {
            answer_json[jvalue] = true;
            machine.randomize();
            machine.resultWinner();
            machine.setStatus(Machine::Status::Stop);
            writeMachine();
        }
    }
    else if (type == "AddBet")
    {
        answer_json[jvalue] = true;
        auto bet = machine.getTotalBet() + 10;
        machine.setTotalBet(bet);
        writeMachine();
    }
    else if (type == "DelBet")
    {
        if (machine.getTotalBet() > 0)
        {
            answer_json[jvalue] = true;
            int bet = machine.getTotalBet() - 10;
            machine.setTotalBet(bet < 0 ? 0 : bet);
            writeMachine();
        }
    }
    else if (type == "SetBet")
    {

        int bet = std::atoi(value.c_str());
        machine.setTotalBet(bet < 0 ? 0 : bet);
        answer_json[jvalue] = true;
        writeMachine();

    }
    else if (type == "LoadContent")
    {
        answer_json[jvalue] = true;
        answer_json["data"] = m_fileMap;
        machine.randomize();
    }

    auto data = QByteArray::fromStdString(answer_json.dump());
    //qDebug() << "responce write " << QString(data);
    responce.setHeader("Content-Type", "application/json");
    responce.write(data);
}
