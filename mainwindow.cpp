#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settingsdialog.h"

#include <iostream>
#include <fstream>

#include <QSettings>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QThread>
#include <QMdiSubWindow>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>


using namespace std;


// --- CONSTRUCTOR ---
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    
    // Connect UI options.
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(ui->actionConfigure, SIGNAL(triggered()), this, SLOT(settingsDialog()));
    connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(loadSession()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newSession()));
    connect(ui->actionEdit, SIGNAL(triggered()), this, SLOT(editSession()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveSession()));
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(connectRemote()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(disconnectRemote()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	connect(ui->actionDonate, SIGNAL(triggered()), this, SLOT(donate()));
    
    // Read configuration.
    QCoreApplication::setApplicationName("MQTTCute");
    QCoreApplication::setApplicationVersion("v0.2");
    QCoreApplication::setOrganizationName("Nyanko");
    QCoreApplication::setOrganizationDomain("www.nyanko.ws");
    QSettings settings;
    defaultSession.mqttHost = settings.value("lastRemote", "localhost").toString().toStdString();
    defaultSession.mqttPort = settings.value("lastPort", 1883).toInt();
    defaultSession.loginType = (LoginType) settings.value("loginType", LOGIN_TYPE_ANONYMOUS).toInt();
    defaultSession.caFile = settings.value("caPath", "").toString().toStdString();
    defaultSession.certFile = settings.value("certPath", "").toString().toStdString();
    defaultSession.keyFile = settings.value("keyPath", "").toString().toStdString();
    currentSession = settings.value("currentSession", "default").toString();
    
    // Add toolbar actions.
    QAction* addTopicAction = ui->mainToolBar->addAction("New topic", this, SLOT(addTopic()));
    QAction* addDiscoveryAction = ui->mainToolBar->addAction("New discovery", this, SLOT(addDiscovery()));
    //addTopicAction->setEnabled(false);
    //addDiscoveryAction->setEnabled(false);
    ui->mainToolBar->setDisabled(true);
    
    // Disable menu options which should be unavailable.
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    
    // Defaults.
    mqtt = 0;
    connected = false;
    validSession = true;
    usingDefaultSession = true;
    loadedSessionPath = "";
    
    // Initialise meta types.
    qRegisterMetaType<string>("string");
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<Session>("Session");
#ifdef USE_NMQTT
    qRegisterMetaType<NmqttBrokerConnection>("NmqttBrokerConnection");
#endif
    
    // Attempt to load the previous session, if any.
    setWindowTitle("MQTTCute - Session: " + currentSession);
}


// --- DECONSTRUCTOR ---
MainWindow::~MainWindow() {
    delete ui;
    if (mqtt) { delete mqtt; }
}


// --- CONNECT REMOTE ---
// Connect to an MQTT broker.
void MainWindow::connectRemote() {
    if (connected) { return; }
    
//    QString remote = QInputDialog::getText(this, tr("MQTT broker"), 
//                                           tr("Server URL or IP"),
//                                           QLineEdit::Normal, remoteServer);
//    if (remote.isEmpty()) { return; }
    
//    int port = QInputDialog::getInt(this, tr("Remote broker port"), 
//                                    tr("Server port"), remotePort);
//    if (remotePort != port || remoteServer != remote) {
//        remotePort = port;
//        remoteServer = remote;
//        QSettings settings;
//        settings.setValue("lastRemote", remoteServer);
//        settings.setValue("lastPort", remotePort);
//    }
    
    // Create MQTT listener and set TLS options if appropriate.
    // TODO: make broker ID configurable.
#ifdef USE_NMQTT
	cout << "Initialising NymphMQTT library...\n";
	mqtt = new NmqttListener(0);
	if (usingDefaultSession) {
		mqtt->init("MQTTCute", QString::fromStdString(defaultSession.mqttHost),
                                defaultSession.mqttPort);
    }
    else {
		mqtt->init("MQTTCute", QString::fromStdString(loadedSession.mqttHost),
                                loadedSession.mqttPort);
    }
#else
    cout << "Initialising Libmosquitto library...\n";
    mosqpp::lib_init();
    if (usingDefaultSession) {
        mqtt = new MqttListener(0, "MQTTCute", QString::fromStdString(defaultSession.mqttHost),
                                defaultSession.mqttPort);
    }
    else {
        mqtt = new MqttListener(0, "MQTTCute", QString::fromStdString(loadedSession.mqttHost),
                                loadedSession.mqttPort);
    }
#endif
    
//    QMessageBox::StandardButton reply = QMessageBox::question(this, 
//                                                     tr("Connect securily"),
//                                                     tr("Connect using TLS?"),
//                                                     QMessageBox::Yes | QMessageBox::No);
//    if (reply == QMessageBox::Yes) {
        // Set TLS options.
//        QString caFile = QInputDialog::getText(this, tr("CA path"), 
//                                               tr("Full path to CA file."),
//                                               QLineEdit::Normal, 
//                                               QString::fromStdString(ca));
//        QString certFile = QInputDialog::getText(this, tr("Cert path"), 
//                                                 tr("Full path to Cert file."),
//                                                 QLineEdit::Normal, 
//                                                 QString::fromStdString(cert));
//        QString keyFile = QInputDialog::getText(this, tr("Key path"), 
//                                                tr("Full path to Key file."),
//                                                QLineEdit::Normal, 
//                                                QString::fromStdString(key));
//        if (caFile.isEmpty() || certFile.isEmpty() || keyFile.isEmpty()) {
//            // Abort connecting.
//            return;
//        }
        
//        QSettings settings;
//        settings.setValue("caFile", caFile);
//        settings.setValue("certFile", certFile);
//        settings.setValue("keyFile", keyFile);
//        ca = caFile.toStdString();
//        cert = certFile.toStdString();
//        key = keyFile.toStdString();
    if (usingDefaultSession && defaultSession.loginType == LOGIN_TYPE_CERTIFICATE) {
		std::cout << "Setting TLS for default session..." << std::endl;
        if (!mqtt->setTLS(defaultSession.caFile, defaultSession.certFile, defaultSession.keyFile)) {
            cerr << "SetTLS failed.\n";
            QMessageBox::critical(this, tr("Setting TLS failed"), 
                                  tr("Setting the TLS parameters failed.\n\n\
                               Please check the provided info and try again."));
            
            return;
        }
    }
    else if (loadedSession.loginType == LOGIN_TYPE_CERTIFICATE) {
		std::cout << "Setting TLS for session..." << std::endl;
        if (!mqtt->setTLS(loadedSession.caFile, loadedSession.certFile, loadedSession.keyFile)) {
            cerr << "SetTLS failed.\n";
            QMessageBox::critical(this, tr("Setting TLS failed"), 
                                  tr("Setting the TLS parameters failed.\n\n\
                               Please check the provided info and try again."));
            
            return;
        }
    }
	
	if (usingDefaultSession && defaultSession.loginType == LOGIN_TYPE_PASSWORD) {
		std::cout << "Setting username/password for default session..." << std::endl;
		if (!mqtt->setPassword(defaultSession.username, defaultSession.password)) {
			cerr << "Setting username/password failed.\n";
            QMessageBox::critical(this, tr("Setting username/password failed"), 
                                  tr("Setting the username and/or password failed.\n\n\
                               Please check the provided info and try again."));
            
            return;
		}
		
	}
	else if (loadedSession.loginType == LOGIN_TYPE_PASSWORD) {
		std::cout << "Setting username/password for session..." << std::endl;
		if (!mqtt->setPassword(loadedSession.username, loadedSession.password)) {
			cerr << "Setting username/password failed.\n";
            QMessageBox::critical(this, tr("Setting username/password failed"), 
                                  tr("Setting the username and/or password failed.\n\n\
                               Please check the provided info and try again."));
            
            return;
		}
	}
    
    // Try to connect. If successful, update local copy and stored remote server address.
    // The MQTT client class has to run on its own thread since it uses its own event loop.
    QThread* thread = new QThread();
    mqtt->moveToThread(thread);
    
#ifdef USE_NMQTT
    connect(mqtt, SIGNAL(newBrokerConnection(NmqttBrokerConnection)), 
			this, SLOT(remoteBrokerConnectResponse(NmqttBrokerConnection)));
#else
    connect(mqtt, SIGNAL(connected()), this, SLOT(remoteConnected()));
#endif
    connect(mqtt, SIGNAL(failed(QString)), this, SLOT(errorHandler(QString)));
    connect(mqtt, SIGNAL(receivedMessage(std::string,std::string)), this, SLOT(receiveMessage(std::string,std::string)));
    connect(thread, SIGNAL(started()), mqtt, SLOT(connectBroker()));
    connect(mqtt, SIGNAL(finished()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), mqtt, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}


// --- DISCONNECT REMOTE ---
// Disconnect from the MQTT broker, if connected.
void MainWindow::disconnectRemote() {
    if (!connected) { return; }
	
	mqtt->disconnectBroker();
	connected = false;
	
	// FIXME: discarding the MQTT runtime here. Should allow for reuse and multiple
	// broker connections instead.
	delete mqtt;
	mqtt = 0;
    
    QMessageBox::information(this, tr("Disconnected."), tr("Disconnected from remote."));
    
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    //addTopicAction->setDisabled(true);
    //addDiscoveryAction->setEnabled(false);
    ui->mainToolBar->setDisabled(true);
}

#ifdef USE_NMQTT
// --- REMOTE BROKER CONNECT RESPONSE ---
// Called when we successfully connected to the MQTT broker.
void MainWindow::remoteBrokerConnectResponse(NmqttBrokerConnection conn) {
	if (conn.responseCode != MQTT_CODE_SUCCESS) {
		QMessageBox::critical(this, tr("Connection error"), tr("Failed to connect to broker."));
		return;
	}
		
    QMessageBox::information(this, tr("Connected"), tr("Successfully connected to the MQTT broker."));
    
    connected = true;
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
//    addTopicAction->setDisabled(false);
//    addDiscoveryAction->setEnabled(true);
    ui->mainToolBar->setEnabled(true);
}

#else
	
// --- REMOTE CONNECTED ---
// Called when we successfully connected to the MQTT broker.
void MainWindow::remoteConnected() {
    QMessageBox::information(this, tr("Connected"), tr("Successfully connected to the MQTT broker."));
    
    connected = true;
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
//    addTopicAction->setDisabled(false);
//    addDiscoveryAction->setEnabled(true);
    ui->mainToolBar->setEnabled(true);
}
#endif



// --- ERROR HANDLER ---
void MainWindow::errorHandler(QString err) {
    QMessageBox::warning(this, tr("Error"), err);
    connected = false;
	
	// Reset UI elements.
	ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->mainToolBar->setDisabled(true);
}


// --- SETTINGS DIALOG ---
// Open the settings dialogue.
void MainWindow::settingsDialog() {
    SettingsDialog sd;
    sd.exec();
}


// --- NEW SESSION ---
void MainWindow::newSession() {
    // Launch empty session dialogue. If accepted, update our local information.
    SessionDialog sd;
    sd.setNewSession(true);
    connect(&sd, SIGNAL(updatedSession(Session, bool)), this, SLOT(updatedSession(Session, bool)));
    sd.exec();
}


// --- LOAD SESSION ---
void MainWindow::loadSession() {
    // Load a serialised session instance from the provided path.
    QString filepath = QFileDialog::getOpenFileName(this, tr("Session file..."), loadedSessionPath);
    if (filepath.isEmpty()) { return; }
    
    ifstream FILE(filepath.toStdString(), ios_base::binary);
    string msg((istreambuf_iterator<char>(FILE)), istreambuf_iterator<char>());
    FILE.close();
    if (msg.length() < 15) { return; }
    
    int index = 0;
    Session s;
    string signature = msg.substr(index, 15);
    index += 15;
    if (signature != "MQTTCUTESESSION") {
        // Abort parsing.
        QMessageBox::critical(this, tr("Load error"), tr("Failed to load session: invalid signature."));
        return;
    }
    
    quint32 msglen = *((quint32*) &msg[index]);
    index += 4;
    if (msg.length() != (19 + msglen)) {
        // Abort parsing.
        QMessageBox::critical(this, tr("Load error"), tr("Failed to load session: invalid message length."));
        return;
    }
    
    quint8 len = *((quint8*) &msg[index++]);
    s.mqttHost = msg.substr(index, len);
    index += len;
    s.mqttPort = *((quint16*) &msg[index]);
    index += 2;
    s.loginType = (LoginType) *((quint8*) &msg[index++]);
    
    // Login type determines what next to parse.
    if (s.loginType == LOGIN_TYPE_ANONYMOUS) {
        // Nothing more to parse.
    }
    else if (s.loginType == LOGIN_TYPE_PASSWORD) {
        // Get the username and password.
        // TODO: decrypt username and password data here.
        len = *((quint8*) &msg[index++]);
        s.username = msg.substr(index, len);
        index += len;
        len = *((quint8*) &msg[index++]);
        s.password = msg.substr(index, len);
        index += len;
    }
    else if (s.loginType == LOGIN_TYPE_CERTIFICATE) {
        quint16 len2 = *((quint16*) &msg[index]);
        index += 2;
        s.caFile = msg.substr(index, len2);
        index += len2;
        
        len2 = *((quint16*) &msg[index]);
        index += 2;
        s.certFile = msg.substr(index, len2);
        index += len2;
        
        len2 = *((quint16*) &msg[index]);
        index += 2;
        s.keyFile = msg.substr(index, len2);
        index += len2;
    }
    else {
        // Unknown type. Abort parsing.
        QMessageBox::critical(this, tr("Load error"), tr("Failed to load session: invalid login type."));
        return;
    }
    
    // TODO: Parse open windows.
        
    loadedSession = s;
    
    usingDefaultSession = false;
    validSession = true;
    currentSession = filepath;
    setWindowTitle("MQTTCute - Session: " + filepath);
}


// --- EDIT SESSION ---
void MainWindow::editSession() {
    SessionDialog sd;
    connect(&sd, SIGNAL(updatedSession(Session, bool)), this, SLOT(updatedSession(Session, bool)));
    if (usingDefaultSession) {
        sd.setSessionData(defaultSession);
    }
    else {
        sd.setSessionData(loadedSession);
    }
    
    sd.exec();
}


// --- SAVE SESSION ---
void MainWindow::saveSession() {
    // Allow the user to save the current session to a file in a serialised format.
    if (usingDefaultSession) { return; }
    
    if (loadedSessionPath.isEmpty()) {
        loadedSessionPath = QFileDialog::getSaveFileName(this, tr("Save"));
        if (loadedSessionPath.isEmpty()) { return; }
    }
    
    cout << "Saving for MQTT host: " << loadedSession.mqttHost << endl;
    
    ofstream FILE(loadedSessionPath.toStdString(), ios_base::binary | ios_base::trunc);
    string signature = "MQTTCUTESESSION";
    string length;
    string out;
    quint8 len;
    len = loadedSession.mqttHost.length();
    out += string((const char*) &len, 1);
    out += loadedSession.mqttHost;
    out += string((const char*) &(loadedSession.mqttPort), 2);
    out += string((const char*) &(loadedSession.loginType), 1);
    if (loadedSession.loginType == LOGIN_TYPE_ANONYMOUS) {
        // Nothing more to write.
    }
    else if (loadedSession.loginType == LOGIN_TYPE_PASSWORD) {
        // Get the username and password.
        // TODO: decrypt username and password data here.
        len = loadedSession.username.length();
        out += string((const char*) &len, 1);
        out += loadedSession.username;
        len = loadedSession.password.length();
        out += string((const char*) &len, 1);
        out += loadedSession.password;
    }
    else if (loadedSession.loginType == LOGIN_TYPE_CERTIFICATE) {
        quint16 len2 = loadedSession.caFile.length();
        out += string((const char*) &len2, 2);
        out += loadedSession.caFile;
        
        len2 = loadedSession.certFile.length();
        out += string((const char*) &len2, 2);
        out += loadedSession.certFile;
            
        len2 = loadedSession.keyFile.length();
        out += string((const char*) &len2, 2);
        out += loadedSession.keyFile;
    }
    
    // Calculate length, write to file.
    quint32 l = out.length();
    length = string((const char*) &l, 4);
    
    FILE.write(signature.data(), signature.length());
    FILE.write(length.data(), length.length());
    FILE.write(out.data(), out.length());
    FILE.close();
    
    savedSession = true;
}


// --- LOAD SESSION ---
// Load an existing session.
//bool MainWindow::loadSession(QString path, Session &s) {
//    //
    
    
//    usingDefaultSession = false;
//    validSession = true;
//}


// --- SAVE SESSION ---
//bool MainWindow::saveSession(QString path, Session &s) {
//    // 
//}


// --- UPDATED SESSION ---
// Called when the session has been updated.
void MainWindow::updatedSession(Session s, bool newSession) {
    cout << "Updating session for broker: " << s.mqttHost << endl;
    
    if (newSession) {
        // Replace the session.
        savedSession = false;
        usingDefaultSession = false;
        loadedSessionPath.clear();
    }
    
    // Check whether we need to replace or update the current session info.
    if (usingDefaultSession) {
        // Just update the QSettings object.
        QSettings settings;
        settings.setValue("lastRemote", QString::fromStdString(s.mqttHost));
        settings.setValue("lastPort", s.mqttPort);
        settings.setValue("loginType", (quint8) s.loginType);
        settings.setValue("caFile", QString::fromStdString(s.caFile));
        settings.setValue("certFile", QString::fromStdString(s.certFile));
        settings.setValue("keyFile", QString::fromStdString(s.keyFile));
        
        defaultSession = s;
    }
    else {        
        // Update session info.
        loadedSession = s;
        
        // Update the saved session information.
        saveSession();
    }
}


// --- ADD TOPIC ---
// Open a new topic window.
void MainWindow::addTopic() {
    QString topic = QInputDialog::getText(this, tr("Name of topic"), 
                                           tr("MQTT topic to add"));
    
    if (topic.isEmpty()) { return; }
    
    // Check that we don't already have this topic.
	topicMutex.lock();
    map<std::string, TopicWindow*>::const_iterator it;
	it = topicwindows.find(topic.toStdString());
	if (it != topicwindows.end()) {
		QMessageBox::warning(this, tr("Existing topic"), tr("The selected topic already exists."));
		topicMutex.unlock();
		return;
	}
    
    // Open a new window in the MDI for this topic.    
    TopicWindow* tw = new TopicWindow(this);
    tw->setTopic(topic);
    topicwindows.insert(std::pair<std::string, TopicWindow*>(topic.toStdString(), tw));
	topicMutex.unlock();
    
    // Add connections.
    connect(tw, SIGNAL(newMessage(std::string,std::string,uint8_t,bool)), 
			mqtt, SLOT(publishMessage(std::string,std::string,uint8_t,bool)));
    connect(tw, SIGNAL(addSubscription(std::string)), mqtt, SLOT(addSubscription(std::string)));
    connect(tw, SIGNAL(removeSubscription(std::string)), mqtt, SLOT(removeSubscription(std::string)));
    connect(tw, SIGNAL(windowClosing(std::string)), this, SLOT(windowClosing(std::string)));
            
    QMdiSubWindow* sw = ui->mdiArea->addSubWindow(tw);
    sw->show();
}


// --- ADD DISCOVERY ---
// Open a new discovery window.
void MainWindow::addDiscovery() {
    QString topic = QInputDialog::getText(this, tr("Name of topic"), 
                                           tr("MQTT topic  ending with '/#'"));
    
    if (topic.isEmpty()) { return; }
    
    // Ensure that the topic name ends with a wildcard,
    // namely '/#'.
    if (!topic.endsWith("/#")) {
		// Throw error.
		QMessageBox::critical(this, tr("Invalid topic"), tr("Discovery topic has to end with '/#'"));
		return;
	}
    
    // Check that we don't already have this topic.
    map<string, DiscoveryWindow*>::const_iterator it;
	discoveryMutex.lock();
	it = discoverywindows.find(topic.toStdString());
	if (it != discoverywindows.end()) {
		QMessageBox::warning(this, tr("Existing topic"), tr("The selected topic already exists."));
		discoveryMutex.unlock();
		return;
	}
    
    // Open a new window in the MDI for this topic.    
    DiscoveryWindow* dw = new DiscoveryWindow(this);
    dw->setTopic(topic);
    discoverywindows.insert(std::pair<std::string, DiscoveryWindow*>(topic.toStdString(), dw));
	discoveryMutex.unlock();
    
    // Add connections.
    connect(dw, SIGNAL(addSubscription(std::string)), mqtt, SLOT(addSubscription(std::string)));
    connect(dw, SIGNAL(removeSubscription(std::string)), mqtt, SLOT(removeSubscription(std::string)));
    connect(dw, SIGNAL(windowClosing(std::string)), this, SLOT(windowClosing(std::string)));
            
    QMdiSubWindow* sw = ui->mdiArea->addSubWindow(dw);
    sw->show();
}


// --- PUBLISH MESSAGE ---
void MainWindow::publishMessage(std::string topic, std::string message, uint8_t qos, bool retain) {
    if (!mqtt) { return; }
    
    mqtt->publishMessage(topic, message, qos, retain);
}


// --- RECEIVE MESSAGE ---
void MainWindow::receiveMessage(std::string topic, std::string message) {
    // Check that we have a window with the appropriate topic.
    map<string, TopicWindow*>::const_iterator it;
	topicMutex.lock();
    it = topicwindows.find(topic);
    bool found = false;
	if (it != topicwindows.end()) {
		found = true;
		it->second->receiveMessage(message);
	}
	
	// Search for partial match (for MQTT # topics).
	for (it = topicwindows.begin(); it != topicwindows.end(); ++it) {
		if (it->first.compare(0, it->first.length() - 1, topic, 0, it->first.length() - 1) == 0) {
			if (it->first[it->first.length() - 1] == '#') {
				// Matching topic found. Send message.
				it->second->receiveMessage(message);
				found = true;
				break;
			}
		}
	}
	
	topicMutex.unlock();
	
	discoveryMutex.lock();
	map<string, DiscoveryWindow*>::const_iterator dit;
	for (dit = discoverywindows.begin(); dit != discoverywindows.end(); ++dit) {
		if (dit->first.compare(0, dit->first.length() - 1, topic, 0, dit->first.length() - 1) == 0) {
			// Matching topic found. Send message.
			dit->second->receiveMessage(topic, message);
			found = true;
			break;
		}
	}
	
	discoveryMutex.unlock();
	
	// If no matching topics were found, try to unsubscribe.
	// FIXME: If the subscription was from an old # topic, figure out a way to unsubscribe anyway.
	if (!found) {
		cerr << "MQTT: message received for unknown topic: " << topic << endl;
	
		// Remove the subscription.
		removeSubscription(topic);
	}
}


// --- ADD SUBSCRIPTION ---
void MainWindow::addSubscription(std::string topic) {
    if (!mqtt) { return; }
    
    mqtt->addSubscription(topic);
}


// --- REMOVE SUBSCRIPTION ---
void MainWindow::removeSubscription(std::string topic) {
    if (!mqtt) { return; }
    
    mqtt->removeSubscription(topic);
}


// --- WINDOW CLOSING ---
void MainWindow::windowClosing(std::string topic) {
    // As the window with the specified topic is closing, we should clean up references to it.
    cout << "Closing window with topic: " << topic << endl;
    map<std::string, TopicWindow*>::iterator it;
	topicMutex.lock();
    it = topicwindows.find(topic);
    if (it == topicwindows.end()) {
        cerr << "Received window closing event for unknown topic: " << topic << endl;
		topicMutex.unlock();
        return;
    }
    
    topicwindows.erase(it);
	topicMutex.unlock();
}


// --- ABOUT ---
// Display an 'about' dialogue.
void MainWindow::about() {
    QMessageBox::about(this, tr("About"), tr("MQTTCute MQTT client v0.3 Alpha.\n(c) 2019 Maya Posch.\nwww.mayaposch.com"));
}


// --- CLOSE EVENT ---
// Handle the window being closed by the user.
void MainWindow::closeEvent(QCloseEvent *event) {
    if (mqtt) {
        mqtt->disconnectBroker();
    }
    
    event->accept();
}


// --- DONATE ---
// Shows the donate dialogue.
void MainWindow::donate() {
	QMessageBox msgBox(this);
	msgBox.setWindowTitle(tr("Donate"));
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setTextFormat(Qt::RichText);
	msgBox.setText(tr("If you like the application and appreciate the effort behind developing and testing it, \nfeel free to donate any amount to the project: <a href=\"http://www.mayaposch.com/donate.php\">MayaPosch.com/donate.php</a>"));
	msgBox.exec();
}


// --- QUIT ---
// Quit the application.
void MainWindow::quit() {
    if (mqtt) {
        mqtt->disconnectBroker();
    }
    
    exit(0);
}
