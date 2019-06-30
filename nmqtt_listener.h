/*
	nmqtt_listener.h - Header file for the NmqttListener class.
	
	Revision 0
	
	Notes:
			- 
			
	2019/06/27, Maya Posch
*/

#ifndef NMQTTLISTENER_H
#define NMQTTLISTENER_H


#include <string>

//#include <nymphmqtt/client.h>
#include "../../NymphMQTT/src/cpp/client.h"

#include <QObject>


class NmqttListener : public QObject {
	Q_OBJECT
	
	NmqttClient client;
	int handle;
	QString host;
	int port;
	
	void messageHandler(int handle, std::string topic, std::string payload);
	void logHandler(int level, std::string text);
	
public:
	explicit NmqttListener(QObject *parent = 0);
    ~NmqttListener();
	
	bool init(QString clientId = "CC-UI", QString host = "localhost", int port = 1883);
	bool setTLS(std::string &ca, std::string &cert, std::string &key);
	bool setPassword(std::string &username, std::string &password);
	void publishMessage(std::string topic, std::string msg);
    void addSubscription(std::string topic);
    void removeSubscription(std::string topic);
    
signals:
    void connected();
	void newBrokerConnection(NmqttBrokerConnection conn);
    void failed(QString err);
    void receivedMessage(std::string topic, std::string message);
    void finished();
    
public slots:
    void connectBroker();
    void disconnectBroker();
};

Q_DECLARE_METATYPE(NmqttBrokerConnection);


#endif
