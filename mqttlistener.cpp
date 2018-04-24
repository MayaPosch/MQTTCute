#include "mqttlistener.h"

#include <iostream>

using namespace std;

#include <QDataStream>
#include <QVector>


// --- CONSTRUCTOR ---
// Connect to the MQTT broker using the provided host and port details.
MqttListener::MqttListener(QObject *parent, QString clientId, QString host, int port) : QObject(parent), mosquittopp(clientId.toStdString().c_str()) {
    this->host = host;
    this->port = port;
}


// --- DECONSTRUCTOR ---
MqttListener::~MqttListener() {   
    mosqpp::lib_cleanup();
    emit finished();
}


// --- SET TLS ---
// Set the CA, Cert and Key file for TLS connections.
bool MqttListener::setTLS(string &ca, string &cert, string &key) {
    cout << "CA: " << ca << ", cert: " << cert << ", key: " << key << ".\n";
        
    int res = tls_set(ca.c_str(), 0, cert.c_str(), key.c_str(), 0);
    if (res != MOSQ_ERR_SUCCESS) { 
        if (res == MOSQ_ERR_INVAL) {
            cerr << "Invalid input parameters for Mosquitto TLS.\n";
        }
        else if (res == MOSQ_ERR_NOMEM) {
            cerr << "Out of memory on Mosquitto TLS.\n";
        }
        else {
            cerr << "Unknown Mosquitto TLS error.\n";
        }
        
        return false; 
    }
    
    
    tls_opts_set(0, 0, 0);  // SSL_VERIFY_NONE
    tls_insecure_set(1);    // insecure connect.
    
    return true;
}


// --- SET PASSWORD ---
// Set the username and password for this MQTT connection.
bool MqttListener::setPassword(string &username, string &password) {
	int res = username_pw_set(username.c_str(), password.c_str());
	if (res != MOSQ_ERR_SUCCESS) {
		if (res == MOSQ_ERR_INVAL) {
            cerr << "Invalid input parameters for Mosquitto PW.\n";
        }
        else if (res == MOSQ_ERR_NOMEM) {
            cerr << "Out of memory on Mosquitto PW.\n";
        }
        else {
            cerr << "Unknown Mosquitto PW error.\n";
        }
        
        return false; 
	}
	
	return true;
}


// --- CONNECT BROKER ---
void MqttListener::connectBroker() {
    cout << "Connecting to broker: " << host.toStdString() << ":" << port << "\n";
    int keepalive = 60;
    mosqpp::mosquittopp::connect(host.toStdString().c_str(), port, keepalive);
    
    int rc;
    while (1) {
        rc = loop();
        if (rc) {
            if (rc == MOSQ_ERR_INVAL) {
                cerr << "Loop: received ERR_INVAL error.\n";
            }
            else if (rc == MOSQ_ERR_NO_CONN) {
                cerr << "Loop: received ERR_NO_CONN.\n";
            }
            else if (rc == MOSQ_ERR_SUCCESS) {
                cerr << "Loop: received ERR_SUCCESS.\n";
            }
            else if (rc == MOSQ_ERR_ERRNO) {
                cerr << "Loop: received ERR_ERRNO.\n";
#ifdef WIN32
                // TODO: Use FormatMessage() to get the error string.
                cerr << "Loop: received error: " << strerror(errno) << endl;
#else
                // TODO: Use strerror_r() to get the error string.
                //cerr << "Loop: system error: " << strerror(errno) << endl;
#endif
            }
            else {
                cerr << "Loop: received error: " << rc << endl;
            }
            
            emit failed("Failed to connect to remote server.");
            break;
            //cout << "Disconnected. Trying to reconnect...\n";
            //reconnect();
        }
    }
}


// --- DISCONNECT BROKER ---
// Disconnect the currently connected broker, if any.
void MqttListener::disconnectBroker() {
    mosqpp::mosquittopp::disconnect();
}


// --- ON CONNECT ---
void MqttListener::on_connect(int rc) {
    cout << "Connected.\n";
            
    if (rc == 0) {
        cout << "Subscribing to topics...\n";
        
        emit connected();
    }
    else {
		// handle.
		cerr << "Connection failed. Aborting subscribing.\n";
        emit failed("Connecting failed. Aborting subscribing");
	}
}


// --- ON SUBSCRIBE ---
void MqttListener::on_subscribe(int mid, int qos_count, const int *granted_qos) {
    // 
}


// --- ON MESSAGE ---
void MqttListener::on_message(const mosquitto_message *message) {
    string topic = message->topic;
    string payload = string((const char*) message->payload, message->payloadlen);
    
    cout << "Received for topic: " << topic << ", message: " << payload << endl;
    emit receivedMessage(topic, payload);
}


// --- ON LOG ---
void MqttListener::on_log(int level, const char *str) {
    cout << "LOG: " << str << ".\n";
}


// --- PUBLISH MESSAGE ---
void MqttListener::publishMessage(string topic, string msg) {
    publish(0, topic.c_str(), msg.length(), msg.c_str());
}


// --- ADD SUBSCRIPTION ---
void MqttListener::addSubscription(string topic) {
    subscribe(0, topic.c_str(), 1);
}


// --- REMOVE SUBSCRIPTION ---
void MqttListener::removeSubscription(string topic) {
    unsubscribe(0, topic.c_str());
}
