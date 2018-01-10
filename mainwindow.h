#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "mqttlistener.h"
#include "topicwindow.h"
#include "discoverywindow.h"
#include "sessiondialog.h"

#include <string>
#include <map>

using namespace std;


// Meta type registration for signal/slots.
Q_DECLARE_METATYPE(string)
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
    void remoteConnected();
    void addTopic();
    void addDiscovery();
    void publishMessage(string topic, string message);
    void receiveMessage(string topic, string message);
    void addSubscription(string topic);
    void removeSubscription(string topic);
    void windowClosing(string topic);
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
    MqttListener* mqtt;
    //string ca, cert, key;
    map<string, TopicWindow*> topicwindows;
    
    //bool loadSession(QString path, Session &s);
    //bool saveSession(QString path, Session &s);
    
protected:
    void closeEvent(QCloseEvent *event);
	map<string, DiscoveryWindow*> discoverywindows;
};

#endif // MAINWINDOW_H
