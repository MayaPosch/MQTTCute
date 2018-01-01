#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

//#include <QtNetwork/QHostAddress>
#include <QSettings>
#include <QMessageBox>
#include <QInputDialog>
#include <QThread>
#include <QMdiSubWindow>


// --- CONSTRUCTOR ---
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    
    // Connect UI options.
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(connectRemote()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(disconnectRemote()));
    
    // Read configuration.
    QCoreApplication::setApplicationName("MQTTCute");
    QCoreApplication::setApplicationVersion("v0.1");
    QCoreApplication::setOrganizationName("Nyanko");
    QCoreApplication::setOrganizationDomain("www.nyanko.ws");
    QSettings settings;
    remoteServer = settings.value("lastRemote", "localhost").toString();
    remotePort = settings.value("lastPort", 1883).toInt();
    ca = settings.value("caPath", "").toString().toStdString();
    cert = settings.value("certPath", "").toString().toStdString();
    key = settings.value("keyPath", "").toString().toStdString();
    
    // Add toolbar actions.
    ui->mainToolBar->addAction("New topic", this, SLOT(addTopic()));
    
    // Defaults.
    mqtt = 0;
    connected = false;
    
    // Initialise meta types.
    qRegisterMetaType<string>("string");
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
    
    QString remote = QInputDialog::getText(this, tr("MQTT broker"), 
                                           tr("Server URL or IP"),
                                           QLineEdit::Normal, remoteServer);
    if (remote.isEmpty()) { return; }
    
    int port = QInputDialog::getInt(this, tr("Remote broker port"), 
                                    tr("Server port"), remotePort);
    if (remotePort != port || remoteServer != remote) {
        remotePort = port;
        remoteServer = remote;
        QSettings settings;
        settings.setValue("lastRemote", remoteServer);
        settings.setValue("lastPort", remotePort);
    }
    
    // Create MQTT listener and set TLS options if appropriate.
    // TODO: make broker ID configurable.
    cout << "Initialising Libmosquitto library...\n";
    mosqpp::lib_init();
    mqtt = new MqttListener(0, "MQTTCute", remoteServer, remotePort);
    
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, 
                                                     tr("Connect securily"),
                                                     tr("Connect using TLS?"),
                                                     QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // Set TLS options.
        QString caFile = QInputDialog::getText(this, tr("CA path"), 
                                               tr("Full path to CA file."),
                                               QLineEdit::Normal, 
                                               QString::fromStdString(ca));
        QString certFile = QInputDialog::getText(this, tr("Cert path"), 
                                                 tr("Full path to Cert file."),
                                                 QLineEdit::Normal, 
                                                 QString::fromStdString(cert));
        QString keyFile = QInputDialog::getText(this, tr("Key path"), 
                                                tr("Full path to Key file."),
                                                QLineEdit::Normal, 
                                                QString::fromStdString(key));
        if (caFile.isEmpty() || certFile.isEmpty() || keyFile.isEmpty()) {
            // Abort connecting.
            return;
        }
        
        QSettings settings;
        settings.setValue("caFile", caFile);
        settings.setValue("certFile", certFile);
        settings.setValue("keyFile", keyFile);
        ca = caFile.toStdString();
        cert = certFile.toStdString();
        key = keyFile.toStdString();
        
        if (!mqtt->setTLS(ca, cert, key)) {
            cerr << "SetTLS failed.\n";
            QMessageBox::critical(this, tr("Setting TLS failed"), 
                                  tr("Setting the TLS parameters failed.\n\n\
                               Please check the provided info and try again."));
            
            return;
        }
    }
    
    // Try to connect. If successful, update local copy and stored remote server address.
    // The MQTT client class has to run on its own thread since it uses its own event loop.
    QThread* thread = new QThread();
    mqtt->moveToThread(thread);
    
    connect(mqtt, SIGNAL(connected()), this, SLOT(remoteConnected()));
    connect(mqtt, SIGNAL(failed(QString)), this, SLOT(errorHandler(QString)));
    connect(mqtt, SIGNAL(receivedMessage(string,string)), this, SLOT(receiveMessage(string,string)));
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
    QMessageBox::information(this, tr("Disconnected."), tr("Disconnected from remote."));
}


// --- REMOTE CONNECTED ---
// Called when we successfully connected to the MQTT broker.
void MainWindow::remoteConnected() {
    QMessageBox::information(this, tr("Connected"), tr("Successfully connected to the MQTT broker."));
}


// --- ERROR HANDLER ---
void MainWindow::errorHandler(QString err) {
    QMessageBox::warning(this, tr("Error"), err);
    connected = false;
}


// --- ADD TOPIC ---
// Open a new topic window.
void MainWindow::addTopic() {
    QString topic = QInputDialog::getText(this, tr("Name of topic"), 
                                           tr("MQTT topic to add"));
    
    if (topic.isEmpty()) { return; }
    
    // Check that we don't already have this topic.
    QList<QMdiSubWindow*> sws = ui->mdiArea->subWindowList();
    for (int i = 0; i < sws.size(); ++i) {
        if (sws[i]->windowTitle() == topic) {
            QMessageBox::warning(this, tr("Existing topic"), tr("The selected topic already exists."));
            return;
        }
    }
    
    // Open a new window in the MDI for this topic.
    //QWidget* w = new QWidget;
    //Ui::TopicWindow tw;
    //tw.setupUi(w);
    
    TopicWindow* tw = new TopicWindow(this);
    tw->setTopic(topic);
    topicwindows.insert(std::pair<string, TopicWindow*>(topic.toStdString(), tw));
    
    // Add connections.
    connect(tw, SIGNAL(newMessage(string,string)), this, SLOT(publishMessage(string,string)));
    connect(tw, SIGNAL(addSubscription(string)), this, SLOT(addSubscription(string)));
    connect(tw, SIGNAL(removeSubscription(string)), this, SLOT(removeSubscription(string)));
            
    QMdiSubWindow* sw = ui->mdiArea->addSubWindow(tw);
    //sw->setWindowTitle(topic);
    sw->show();
}


// --- PUBLISH MESSAGE ---
void MainWindow::publishMessage(string topic, string message) {
    if (!mqtt) { return; }
    
    mqtt->publishMessage(topic, message);
}


// --- RECEIVE MESSAGE ---
void MainWindow::receiveMessage(string topic, string message) {
    // Check that we have a window with the appropriate topic.
    map<string, TopicWindow*>::const_iterator it;
    it = topicwindows.find(topic);
    if (it == topicwindows.end()) {
        cerr << "MQTT: message received for unknown topic: " << topic << endl;
        
        // Remove the subscription.
        removeSubscription(topic);
        
        return;
    }
    
    it->second->receiveMessage(message);
}


// --- ADD SUBSCRIPTION ---
void MainWindow::addSubscription(string topic) {
    if (!mqtt) { return; }
    
    mqtt->addSubscription(topic);
}


// --- REMOVE SUBSCRIPTION ---
void MainWindow::removeSubscription(string topic) {
    if (!mqtt) { return; }
    
    mqtt->removeSubscription(topic);
}


// --- WINDOW CLOSING ---
void MainWindow::windowClosing(string topic) {
    // As the window with the specified topic is closing, we should clean up references to it.
    map<string, TopicWindow*>::iterator it;
    it = topicwindows.find(topic);
    if (it == topicwindows.end()) {
        cerr << "Received window closing event for unknown topic: " << topic << endl;
        return;
    }
    
    topicwindows.erase(it);
}


// --- ABOUT ---
// Display an 'about' dialogue.
void MainWindow::about() {
    QMessageBox::about(this, tr("About"), tr("MQTTCute MQTT client v0.1 Alpha.\n(c) 2018 Maya Posch.\nwww.mayaposch.com"));
}


// --- QUIT ---
// Quit the application.
void MainWindow::quit() {
    if (mqtt) {
        mqtt->disconnectBroker();
    }
    
    exit(0);
}
