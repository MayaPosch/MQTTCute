/*
	nmqtt_listener.cpp - Implementation file for the MqttListener class.
	
	Revision 0
	
	Features:
			-
	
	Notes:
			- 
			
	2019/06/27, Maya Posch
*/


#include "nmqtt_listener.h"

// debug
#include <iostream>


// --- CONSTRUCTOR ---
NmqttListener::NmqttListener(QObject *parent) : QObject(parent)  {
	using namespace std::placeholders;
	client.init(std::bind(&NmqttListener::logHandler, this, _1, _2), NYMPH_LOG_LEVEL_TRACE, 3000);
	client.setMessageHandler(std::bind(&NmqttListener::messageHandler, this, _1, _2, _3));
}


// --- DESTRUCTOR ---
NmqttListener::~NmqttListener() {
	client.shutdown();
	emit finished();
}


// --- MESSAGE HANDLER ---
void NmqttListener::messageHandler(int handle, std::string topic, std::string payload) {
	// TODO: use the handle parameter.
	emit receivedMessage(topic, payload);
}


// --- LOG HANDLER ---
void NmqttListener::logHandler(int level, std::string text) {
	std::cout << level << " - " << text << std::endl;
}


// -- INIT ---
bool NmqttListener::init(QString clientId, QString host, int port) {
	this->host = host;
	this->port = port;
	client.setClientId(clientId.toStdString());
	
	return true;
}


// --- SET TLS ---
bool NmqttListener::setTLS(std::string &ca, std::string &cert, std::string &key) {
	//
	
	return true;
}


// --- SET PASSWORD ---
bool NmqttListener::setPassword(std::string &username, std::string &password) {
	//
	
	return true;
}


// --- PUBLISH MESSAGE ---
void NmqttListener::publishMessage(std::string topic, std::string msg) {
	std::string result;
	if (!client.publish(handle, topic, msg, result)) { // QoS 0.
		emit failed(QString::fromStdString(result));
	}
}


// --- ADD SUBSCRIPTION ---
void NmqttListener::addSubscription(std::string topic) {
	std::string result;
	if (!client.subscribe(handle, topic, result)) {
		emit failed(QString::fromStdString(result));
	}
}


// --- REMOVE SUBSCRIPTION ---
void NmqttListener::removeSubscription(std::string topic) {
	std::string result;
	if (!client.unsubscribe(handle, topic, result)) {
		emit failed(QString::fromStdString(result));
	}
}


// --- CONNECT BROKER ---
void NmqttListener::connectBroker() {
	std::string result;
	NmqttBrokerConnection conn;
	if (!client.connect(host.toStdString(), port, handle, 0, conn, result)) {
		emit failed(QString::fromStdString(result));
		return;
	}
	
	emit newBrokerConnection(conn);
}


// --- DISCONNECT BROKER ---
void NmqttListener::disconnectBroker() {
	std::string result;
	client.disconnect(handle, result);
}
	
