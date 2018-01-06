#include "discoverywindow.h"
#include "ui_discoverywindow.h"

#include <QScrollBar>


// --- CONSTRUCTOR ---
DiscoveryWindow::DiscoveryWindow(QWidget *parent) : QWidget(parent),
    ui(new Ui::DiscoveryWindow) {
    ui->setupUi(this);
	
	// Connections.
    connect(ui->subscribeCheckBox, SIGNAL(toggled(bool)), this, SLOT(subscriptionStatus(bool)));
    
    // Defaults
    ui->subscribeCheckBox->setChecked(false);
    ui->subscribeTextEdit->document()->setMaximumBlockCount(100); // Limit paragraphs.
}


// --- DECONSTRUCTOR ---
DiscoveryWindow::~DiscoveryWindow() {
    delete ui;
}


// --- SET TOPIC ---
void DiscoveryWindow::setTopic(QString topic) {
    this->topic = topic.toStdString();
    setWindowTitle(topic);
}


// --- SUBSCRIPTION STATUS ---
void DiscoveryWindow::subscriptionStatus(bool status) {
    if (status) { emit addSubscription(topic); }
    else { emit removeSubscription(topic); }
}


// --- RECEIVE MESSAGE ---
// Called when a new message is available.
void DiscoveryWindow::receiveMessage(string topic, string message) {
	set<string>::const_iterator it;
	it = topics.find(topic);
	if (it == topics.end()) {
		// Topic not found. Insert and add to GUI.
		topics.insert(topic);
		QString text = QString::fromStdString(topic) + "\n";
		// Scroll to bottom after inserting text if we are already at the bottom, else leave the scrollbar where it is.
		if (ui->subscribeTextEdit->verticalScrollBar()->value() == ui->subscribeTextEdit->verticalScrollBar()->maximum()) {
			ui->subscribeTextEdit->insertPlainText(text);
			ui->subscribeTextEdit->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
		}
		else {
			ui->subscribeTextEdit->insertPlainText(text);
		}
	}
}


// --- CLOSE EVENT ---
// Handle the window being closed by the user.
void DiscoveryWindow::closeEvent(QCloseEvent *event) {
    emit windowClosing(topic);
    event->accept();
}
