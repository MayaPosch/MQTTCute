#-------------------------------------------------
#
# Project created by QtCreator 2017-12-24T16:50:07
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MQTTCute
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    mqttlistener.cpp \
    topicwindow.cpp \
    discoverywindow.cpp

win32-g++ {
    SOURCES += mosquitto/lib/cpp/mosquittopp.cpp \
    mosquitto/lib/logging_mosq.c \
    mosquitto/lib/memory_mosq.c \
    mosquitto/lib/messages_mosq.c \
    mosquitto/lib/mosquitto.c \
    mosquitto/lib/net_mosq.c \
    mosquitto/lib/read_handle.c \
    mosquitto/lib/read_handle_client.c \
    mosquitto/lib/read_handle_shared.c \
    mosquitto/lib/send_client_mosq.c \
    mosquitto/lib/send_mosq.c \
    mosquitto/lib/socks_mosq.c \
    mosquitto/lib/srv_mosq.c \
    mosquitto/lib/thread_mosq.c \
    mosquitto/lib/time_mosq.c \
    mosquitto/lib/tls_mosq.c \
    mosquitto/lib/util_mosq.c \
    mosquitto/lib/will_mosq.c
}

HEADERS  += mainwindow.h \
    mqttlistener.h \
    topicwindow.h \
    discoverywindow.h

win32-g++ {
    HEADERS += mosquitto/lib/cpp/mosquittopp.h \
    mosquitto/lib/dummypthread.h \
    mosquitto/lib/logging_mosq.h \
    mosquitto/lib/memory_mosq.h \
    mosquitto/lib/messages_mosq.h \
    mosquitto/lib/mosquitto.h \
    mosquitto/lib/mosquitto_internal.h \
    mosquitto/lib/mqtt3_protocol.h \
    mosquitto/lib/net_mosq.h \
    mosquitto/lib/read_handle.h \
    mosquitto/lib/send_mosq.h \
    mosquitto/lib/socks_mosq.h \
    mosquitto/lib/time_mosq.h \
    mosquitto/lib/tls_mosq.h \
    mosquitto/lib/util_mosq.h \
    mosquitto/lib/will_mosq.h
}

FORMS    += mainwindow.ui \
    topicwindow.ui \
    discoverywindow.ui

!win32 {
    LIBS += -lmosquitto -lmosquittopp
}

win32:CONFIG(release, debug|release): LIBS += -lws2_32
win32:CONFIG(debug, debug|release): LIBS += -lws2_32
win32-msvc:CONFIG(release, debug|release): LIBS += -L$$PWD/mosquitto/msvc_2017_x64/
win32-msvc:CONFIG(debug, debug|release): LIBS += -L$$PWD/mosquitto/msvc_2017_x64/

#INCLUDEPATH += $$PWD/mosquitto
#DEPENDPATH += $$PWD/mosquitto
