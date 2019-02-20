TARGERT = Server

QT -= gui
QT += core network

CONFIG += c++11 c++14 c++17 console
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=gnu++17


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    requesthandler.cpp \
    ../../Classes/Machine.cpp



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QtWebApp/release/ -lQtWebApp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QtWebApp/debug/ -lQtWebAppd
else:unix: LIBS += -L$$OUT_PWD/../QtWebApp/ -lQtWebApp

INCLUDEPATH += $$PWD/../QtWebApp/httpserver
DEPENDPATH += $$PWD/../QtWebApp/httpserver
INCLUDEPATH += $$PWD/../QtWebApp/templateengine
DEPENDPATH += $$PWD/../QtWebApp/templateengine
INCLUDEPATH += $$PWD/../QtWebApp/qtservice
DEPENDPATH += $$PWD/../QtWebApp/qtservice


win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtWebApp/release/libQtWebApp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtWebApp/debug/libQtWebAppd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtWebApp/release/QtWebApp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtWebApp/debug/QtWebAppd.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../QtWebApp/libQtWebApp.a

INCLUDEPATH += $$PWD/../../Classes

HEADERS += \
    configserver.h \
    requesthandler.h \
    ../../Classes/Machine.h

RESOURCES += \
    res_sprites.qrc


#DEBUG
#C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe
#INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/ucrt"
#LIBS += -L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.10240.0/ucrt/x64"
