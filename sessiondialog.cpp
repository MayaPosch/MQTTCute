#include "sessiondialog.h"
#include "ui_sessiondialog.h"

#include <QFileDialog>


// --- CONSTRUCTOR ---
SessionDialog::SessionDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SessionDialog) {
    ui->setupUi(this);
    
    // Connections.
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(okButton()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelButton()));
    connect(ui->caFilePushButton, SIGNAL(pressed()), this, SLOT(browseCa()));
    connect(ui->certFilePushButton, SIGNAL(pressed()), this, SLOT(browseCert()));
    connect(ui->keyFilePushButton, SIGNAL(pressed()), this, SLOT(browseKey()));
    
    // Defaults.
    newSession = false;
}


// --- DECONSTRUCTOR ---
SessionDialog::~SessionDialog() {
    delete ui;
}


// --- SET NEW SESSION ---
// Sets whether this is a new session. This value will be passed along upon completion of this 
// dialogue.
void SessionDialog::setNewSession(bool newSession) {
    this->newSession = newSession;
}


// --- SET SESSION DATA ---
void SessionDialog::setSessionData(Session s) {
    // Set the UI fields with the provided data.
    ui->mqttAddressEdit->setText(QString::fromStdString(s.mqttHost));
    ui->mqttportSpinBox->setValue(s.mqttPort);
    ui->loginMethodComboBox->setCurrentIndex((int) s.loginType);
    ui->usernameLineEdit->setText(QString::fromStdString(s.username));
    ui->passwordLineEdit->setText(QString::fromStdString(s.password));
    ui->caFileEdit->setText(QString::fromStdString(s.caFile));
    ui->certFileEdit->setText(QString::fromStdString(s.certFile));
    ui->keyFileEdit->setText(QString::fromStdString(s.keyFile));
}


// --- OK BUTTON ---
void SessionDialog::okButton() {
    setResult(QDialog::Accepted);
    Session s;
    s.mqttHost = ui->mqttAddressEdit->text().toStdString();
    s.mqttPort = ui->mqttportSpinBox->value();
    s.loginType = (LoginType) ui->loginMethodComboBox->currentIndex();
    s.username = ui->usernameLineEdit->text().toStdString();
    s.password = ui->passwordLineEdit->text().toStdString();
    s.caFile = ui->caFileEdit->text().toStdString();
    s.certFile = ui->certFileEdit->text().toStdString();
    s.keyFile = ui->keyFileEdit->text().toStdString();
    emit updatedSession(s, newSession);
    close();
}


// --- CANCEL BUTTON ---
void SessionDialog::cancelButton() {
    setResult(QDialog::Rejected);
    close();
}


// --- BROWSE CA ---
void SessionDialog::browseCa() {
    QString dir = ui->caFileEdit->text();
    QString filepath = QFileDialog::getOpenFileName(this, tr("CA file"), dir);
    if (filepath.isEmpty()) { return; }
    
    ui->caFileEdit->setText(filepath);
}


// --- BROWSE CERT ---
void SessionDialog::browseCert() {
    QString dir = ui->certFileEdit->text();
    QString filepath = QFileDialog::getOpenFileName(this, tr("Cert file"), dir);
    if (filepath.isEmpty()) { return; }
    
    ui->certFileEdit->setText(filepath);
}


// --- BROWSE KEY ---
void SessionDialog::browseKey() {
    QString dir = ui->keyFileEdit->text();
    QString filepath = QFileDialog::getOpenFileName(this, tr("Key file"), dir);
    if (filepath.isEmpty()) { return; }
    
    ui->keyFileEdit->setText(filepath);
}
