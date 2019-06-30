#include "topicwindow.h"
#include "ui_topicwindow.h"

#include <iostream>

#include <QScrollBar>


// --- CONSTRUCTOR ---
TopicWindow::TopicWindow(QWidget *parent) : QWidget(parent), ui(new Ui::TopicWindow) {
    ui->setupUi(this);
    
    // Connections.
    connect(ui->publishButton, SIGNAL(pressed()), this, SLOT(publishTopic()));
    connect(ui->publishLineEdit, SIGNAL(returnPressed()), this, SLOT(publishTopic()));
    connect(ui->subscribeCheckBox, SIGNAL(toggled(bool)), this, SLOT(subscriptionStatus(bool)));
    
    // Defaults
    ui->subscribeCheckBox->setChecked(false);
    ui->subscribeTextEdit->document()->setMaximumBlockCount(100); // Limit paragraphs.
    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    ui->subscribeTextEdit->setFont(font);
    this->setFixedWidth(320);
}


// --- SET TOPIC ---
void TopicWindow::setTopic(QString topic) {
    this->topic = topic.toStdString();
    setWindowTitle(topic);
}


// --- DECONSTRUCTOR ---
TopicWindow::~TopicWindow() {
    delete ui;
}


// --- PUBLISH TOPIC ---
void TopicWindow::publishTopic() {
    std::string message = ui->publishLineEdit->text().toStdString();
    
    // Parse any binary meta characters in the message string.
    // These start with \x and are followed by two digits (0-9, a-f, A-F).
    int idx = 0;
    uint outpos = 0;
    std::string out;
    while ((idx = message.find("\\x", idx)) != std::string::npos) {
        // Read the next two characters into a string for conversion.
        // Copy the part that will remain unchanged into the output string.
        out += message.substr(outpos, (idx - outpos));
        QString hex = QString::fromStdString(message.substr((idx + 2), 2));
        bool ok;
        quint8 tint = (quint8) hex.toUInt(&ok, 16);
        
        //cout << "Index: " << idx << ", tInt: " << tint << endl;
        
        // if !ok, copy the four characters verbatim.
        if (!ok) {
            out += message.substr(idx, 4);
        }
        else {
            out += std::string((const char*) &tint, 1);
        }
        
        // Update indices.
        outpos = idx + 4;
        idx += 4;
    }
    
    //cout << "Outpos: " << outpos << ", MsgLength: " << message.length() << endl;
    //cout << "Out: " << out << endl;
    //cout << "End: " << message.substr(outpos) << endl;
    
    if (out.length() < 1) { out = message; }
    else if (message.length() > outpos) { out += message.substr(outpos); }
    
    emit newMessage(topic, out);
}


// --- SUBSCRIPTION STATUS ---
void TopicWindow::subscriptionStatus(bool status) {
    if (status) { emit addSubscription(topic); }
    else { emit removeSubscription(topic); }
}


// --- RECEIVE MESSAGE ---
// Called when a new message is available.
void TopicWindow::receiveMessage(std::string message) {
    // Format to hex-editor style text, with 8 hex bytes & 8 ASCII characters per line.
    QString text;
    uint i = 0;
    while (i < message.length()) {
        std::string temp;
        if ((i + 8) < message.length()) { temp = message.substr(i, 8); }
        else { temp = message.substr(i); }
        
        QString line;
        for (uint j = 0; j < temp.length(); ++j) {
            QString l = QString::number((uint) temp[j], 16);
            if (l.length() == 1) { line += "0" + l; }
            else { line += l; }
            
            line += " ";
        }
        
        if (line.length() < 24) {
            // Add padding.
            while (line.length() < 24) { line += "\u2007"; } // using U+2007   FIGURE SPACE
        }
        
        text += line;
        text += "\u2007\u2007\u2007\u2007";
        text += QString::fromStdString(temp);
        text += "\n";
        
        i += 8;
    }
    
    // Scroll to bottom after inserting text if we are already at the bottom, else leave the scrollbar where it is.
    if (ui->subscribeTextEdit->verticalScrollBar()->value() == ui->subscribeTextEdit->verticalScrollBar()->maximum()) {
        ui->subscribeTextEdit->insertPlainText(text);
        ui->subscribeTextEdit->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
    }
    else {
        ui->subscribeTextEdit->insertPlainText(text);
    }
}


// --- CLOSE EVENT ---
// Handle the window being closed by the user.
void TopicWindow::closeEvent(QCloseEvent *event) {
    emit windowClosing(topic);
    event->accept();
}
