#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//#include <mqtt/qmqtt.h>
#include "mqttlistener.h"
#include "topicwindow.h"

#include <string>
#include <map>

using namespace std;


// Meta type registration for signal/slots.
Q_DECLARE_METATYPE(string)


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void connectRemote();
    void disconnectRemote();
    void remoteConnected();
    void addTopic();
    void publishMessage(string topic, string message);
    void receiveMessage(string topic, string message);
    void addSubscription(string topic);
    void removeSubscription(string topic);
    void windowClosing(string topic);
    void errorHandler(QString err);
    void about();
    void quit();
    
private:
    Ui::MainWindow *ui;
    bool connected;
    QString remoteServer;
    int remotePort;
    MqttListener* mqtt;
    string ca, cert, key;
    map<string, TopicWindow*> topicwindows;
    //QMQTT::Client* mqtt_client;
};

#endif // MAINWINDOW_H
