#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H
#include <httprequesthandler.h>

#include <vector>
#include <map>
#include <utility>
#include <functional>

#include <QHash>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QDebug>

#include "Machine.h"



class RequestHandler : public stefanfrings::HttpRequestHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(RequestHandler)
public:
    inline RequestHandler(QObject *p) :
        stefanfrings::HttpRequestHandler(p)
    {
        init();
    }

    virtual ~RequestHandler();
    virtual void service(stefanfrings::HttpRequest& request, stefanfrings::HttpResponse& response) override;

    using BinnaryVector = std::vector<uint8_t>; //std::vector<std::byte>;
    using BinnaryMap = std::map<std::string, BinnaryVector>;

private:
    void init();
    void processResponce(const QString &guid, const std::string &type, const std::string &value, stefanfrings::HttpResponse &responce);

    QHash<QString, Machine> m_clients;
    BinnaryMap m_fileMap;

};


#endif // REQUESTHANDLER_H
