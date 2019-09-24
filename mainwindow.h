#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#ifdef USE_NMQTT
#include "nmqtt_listener.h"
#else
#include "mqttlistener.h"
#endif

#include "topicwindow.h"
#include "discoverywindow.h"
#include "sessiondialog.h"

#include <string>
#include <map>
#include <mutex>


// Meta type registration for signal/slots.
Q_DECLARE_METATYPE(string)
Q_DECLARE_METATYPE(uint8_t)
Q_DECLARE_METATYPE(Session)


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void settingsDialog();
    void newSession();
    void loadSession();
    void editSession();
    void saveSession();
    void connectRemote();
    void disconnectRemote();
#ifdef USE_NMQTT
	void remoteBrokerConnectResponse(NmqttBrokerConnection conn);
#else
    void remoteConnected();
#endif
    void addTopic();
    void addDiscovery();
    void publishMessage(std::string topic, std::string message, uint8_t qos = 0, bool retain = false);
    void receiveMessage(std::string topic, std::string message);
    void addSubscription(std::string topic);
    void removeSubscription(std::string topic);
    void windowClosing(std::string topic);
    void errorHandler(QString err);
    void about();
    void quit();
    
public slots:
    void updatedSession(Session s, bool newSession);
    
private:
    Ui::MainWindow *ui;
    QAction* addTopicAction;
    QAction* addDiscoveryAction;
    bool connected;
    Session defaultSession;
    Session loadedSession;
    QString loadedSessionPath;
    bool validSession;
    bool savedSession;
    bool usingDefaultSession;
    QString currentSession;
    //QString remoteServer;
    //int remotePort;
#ifdef USE_NMQTT
	NmqttListener* mqtt;
#else
    MqttListener* mqtt;
#endif
    //string ca, cert, key;
    std::map<std::string, TopicWindow*> topicwindows;
	std::mutex topicMutex;
    
    //bool loadSession(QString path, Session &s);
    //bool saveSession(QString path, Session &s);
    
protected:
    void closeEvent(QCloseEvent *event);
	std::map<std::string, DiscoveryWindow*> discoverywindows;
	std::mutex discoveryMutex;
};

#endif // MAINWINDOW_H
