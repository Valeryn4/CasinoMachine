#include <QCoreApplication>
#include <QDebug>
#include <QTextStream>
#include <QTextCodec>

#include <httplistener.h>


#include "configserver.h"
#include "requesthandler.h"

int main(int argc, char *argv[])
{
       setlocale(LC_CTYPE, "rus"); // вызов функции настройки локали

    QCoreApplication a(argc, argv);


    auto settings = new HttpSettings("config.ini", &a);
    auto handler = new RequestHandler(&a);
    auto listener = new stefanfrings::HttpListener(settings, handler, &a);

    Q_UNUSED(listener);

    return a.exec();
}
