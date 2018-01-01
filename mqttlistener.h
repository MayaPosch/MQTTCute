#ifndef MQTTLISTENER_H
#define MQTTLISTENER_H

// Differentiate between three categories of compilers for this include:
// * MSVC 2017 x64  -> Use provided header & .lib files.
// * MinGW          -> Compile the entire source for direct inclusion.
// * Other          -> Use the globally installed libmosquitto(pp) files.
#ifdef __MINGW32__
#include "mosquitto/lib/cpp/mosquittopp.h"
#elif defined __MINGW64__
#include "mosquitto/lib/cpp/mosquittopp.h"
#elif defined _MSC_VER
#ifdef _WIN64
#include "mosquitto/mosquittopp.h"
//#pragma comment(linker, "/LIBPATH:"
#pragma comment(lib, "mosquitto.lib")
#pragma comment(lib, "mosquittopp.lib")
#endif
#else
#include <mosquittopp.h>
#endif


#include <string>

using namespace std;

#include <QObject>


class MqttListener : public QObject, mosqpp::mosquittopp {
    Q_OBJECT
    
    QString host;
    int port;
    
public:
    explicit MqttListener(QObject *parent = 0, QString clientId = "CC-UI", 
                          QString host = "localhost", int port = 1883);
    ~MqttListener();
    
    bool setTLS(string &ca, string &cert, string &key);
    void on_connect(int rc);
    void on_message(const struct mosquitto_message* message);
    void on_subscribe(int mid, int qos_count, const int* granted_qos);
    void on_log(int level, const char* str);
    void publishMessage(string topic, string msg);
    void addSubscription(string topic);
    void removeSubscription(string topic);
    
signals:
    void connected();
    void failed(QString err);
    void receivedMessage(string topic, string message);
    void finished();
    
public slots:
    void connectBroker();
    void disconnectBroker();
};

#endif // MQTTLISTENER_H
